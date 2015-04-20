//
//  MessageDispatcher.hpp
//  Overview
//
//  Created by Samir Sinha on 11/15/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//


#include "messagedispatcher.hpp"
#include "messagestream.hpp"

#include <cinek/debug.hpp>
#include <capnp/serialize.h>

namespace cinek { namespace overview {

MessageDispatcher::MessageDispatcher(size_t delegateCount, size_t classCount,
                                     size_t sequenceCount,
                                     Allocator allocator) :
    _nextSeqId(0),
    _nextDelegateHandle(0),
    _delegatePool(delegateCount + classCount,  // one head node per class
                  allocator),
    _delegates(allocator),
    _classDelegates(allocator),
    _sequenceDelegates(allocator)
{
    _delegates.reserve(delegateCount);
    _classDelegates.reserve(classCount);
    _sequenceDelegates.reserve(sequenceCount);
}

MessageDispatcher::~MessageDispatcher()
{
    //  release master list of delegates
    for (auto& del : _delegates)
    {
        if (del.second)
        {
            _delegatePool.destruct(del.second);
        }
        del.second = nullptr;
    }
    //  release head list nodes from the class delegates table
    for (auto& classDel : _classDelegates)
    {
        if (classDel.second)
        {
            _delegatePool.destruct(classDel.second);
        }
        classDel.second = nullptr;
    }
}

MessageDelegateHandle MessageDispatcher::queue
(
    MessageStream& stream,
    capnp::MessageBuilder& msg,
    const MessageHeader::ClassId& msgClass,
    const Delegate& cb
)
{
    if (cb)
    {
        //  create sequence delegate
        DelegateEntry* entry = _delegatePool.construct();
        if (!entry)
            return 0;

        obtainDelegateEntry(entry);

        ++_nextDelegateHandle;
        if (!_nextDelegateHandle)
            _nextDelegateHandle = 1;

        auto inserter = _delegates.emplace(_nextDelegateHandle, entry);
        if (!inserter.second)
        {
            // collisions are highly unlikely given the range of delegate handle
            releaseDelegateEntry(entry);
            return 0;
        }
        entry->handle = _nextDelegateHandle;

        ++_nextSeqId;
        if (!_nextSeqId)
            _nextSeqId = 1;

        auto seqInserter = _sequenceDelegates.emplace(_nextSeqId, entry);
        if (!seqInserter.second)
        {
            //  again, collisions are highly unlikely unless the caller hasn't
            //  been processing messages regularly for a long time.
            releaseDelegateEntry(entry);
            return 0;
        }
        entry->del = cb;
        entry->seqId = _nextSeqId;
    }

    MessageHeader header;
    header.classId = msgClass;
    header.type = MessageHeader::kCommand;
    header.seqId = _nextSeqId;
    writeMessageHeader(stream, header);
    writeMessage(stream, msg);
    
    return cb ? _nextDelegateHandle : 0;
}

MessageDelegateHandle MessageDispatcher::on
(
    const MessageHeader::ClassId& id,
    const Delegate& del
)
{
    auto it = lowerBoundIterator(id, _classDelegates);

    //  add a head list node for the class
    if (it == _classDelegates.end() || it->first != id)
    {
        DelegateEntry* head = _delegatePool.construct();
        if (!head)
        {
            CK_LOG_ERROR("MessageDispatcher", "Failed to allocate head object. "
                                              "Abort subscribe.");
            return 0;
        }
        obtainDelegateEntry(head);
        head->next = head;
        head->prev = head;
        it = _classDelegates.emplace(it, id, head);
    }

    if (it == _classDelegates.end())
    {
        CK_LOG_ERROR("MessageDispatcher", "Failed to insert head object. "
                                          "Abort subscribe.");
        return 0;
    }

    DelegateEntry* entry = _delegatePool.construct();
    if (!entry)
    {
        CK_LOG_ERROR("MessageDispatcher", "Failed to create entry. "
                                          "Abort subscribe.");
        return 0;
    }
    obtainDelegateEntry(entry);

    ++_nextDelegateHandle;
    if (!_nextDelegateHandle)
        _nextDelegateHandle = 1;

    auto inserter = _delegates.emplace(_nextDelegateHandle, entry);
    if (!inserter.second)
    {
        // collisions are highly unlikely given the range of delegate handle
        releaseDelegateEntry(entry);
        return 0;
    }

    entry->del = del;
    entry->next = it->second;
    entry->prev = it->second->prev;
    entry->prev->next = entry;
    entry->next->prev = entry;

    return _nextDelegateHandle;
}

void MessageDispatcher::remove(MessageDelegateHandle delegateHandle)
{
    //  note, this is only for subscribed events.  commands cannot be cancelled
    auto it = _delegates.find(delegateHandle);
    if (it == _delegates.end())
        return;

    auto entry = it->second;
    entry->handle = 0;
    _delegates.erase(it);

    //  releasing the delegate will clear it automatically from the
    //  classDelegates delegate list (while maintaining the class entry in the
    //  table for optimization/reuse).
    releaseDelegateEntry(entry);
}

void MessageDispatcher::dispatch(MessageStream& stream, void* context)
{
    MessageHeader hdr;
    do
    {
        readMessageHeader(stream, hdr);
        if (hdr.type <= MessageHeader::kEnd)
            continue;
   
        capnp::InputStreamMessageReader reader(stream);
        
        if (hdr.seqId)
        {
            auto it = _sequenceDelegates.find(hdr.seqId);
            if (it != _sequenceDelegates.end())
            {
                auto entry = it->second;
                {
                    obtainDelegateEntry(entry);
                    entry->del(reader, entry->seqId, context);
                    //  invalidate sequence entry before releasing to prevent an
                    //  unnecessary lookup of the entry during release.
                    releaseDelegateEntry(entry);
                    entry->seqId = 0;
                    _sequenceDelegates.erase(it);
                }
                //  done with the delegate.  release the dispatcher's instance
                releaseDelegateEntry(entry);
            }
        }
        else
        {
            //  dispatch to any class-based listeners
            auto it = lowerBoundIterator(hdr.classId, _classDelegates);
            if (it == _classDelegates.end() || it->first != hdr.classId)
                return;

            auto head = it->second;
            auto entry = head->next;
            while (entry != head)
            {
                //  handle case where delegate deregisters the current delegate
                obtainDelegateEntry(entry);

                entry->del(reader, 0, context);
                auto next = entry->next;

                releaseDelegateEntry(entry);

                entry = next;
            }
        }
    }
    while(hdr.type > MessageHeader::kEnd);
}

void MessageDispatcher::obtainDelegateEntry(DelegateEntry* entry)
{
    ++entry->refcnt;
}

void MessageDispatcher::releaseDelegateEntry(DelegateEntry* entry)
{
    if (entry->refcnt > 0)
    {
        --entry->refcnt;
        if (!entry->refcnt)
        {
            //  Clear from the _classDelegates delegate list.  The head list
            //  node is not deleted until dispatcher destruction.
            if (entry->prev)
            {
                entry->prev->next = entry->next;
            }
            if (entry->next)
            {
                entry->next->prev = entry->prev;
            }
            if (entry->seqId)
            {
                _sequenceDelegates.erase(entry->seqId);
            }
            if (entry->handle)
            {
                _delegates.erase(entry->handle);
            }
            _delegatePool.destruct(entry);
        }
    }
}

} /* namespace overview */ } /* namespace cinek */
