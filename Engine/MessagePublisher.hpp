///
/// @file MessagePublisher.hpp
///
/// @author Samir Sinha
/// @date 11/15/14.
/// @copyright CineKine. All rights reserved.
///

#ifndef Overview_Game_MessagePublisher_hpp
#define Overview_Game_MessagePublisher_hpp

#include "MessageStream.hpp"
#include "ObjectTypes.hpp"

#include <cinek/objectpool.hpp>
#include <cinek/vector.hpp>
#include <cinek/map.hpp>
#include <cinek/debug.hpp>
#include <capnp/message.h>
#include <capnp/serialize.h>


namespace cinek { namespace overview {

/// @class MessagePublisher
/// @brief Implements queuing and dispatch of messages from a MessageStream
///        using the Cap'n Proto messaging system
///

/*

    using Delegate =
        std::function<void(const MessageHeader::ClassId&,
                           capnp::MessageReader&,
                           MessageHeader::SequenceId)>;

*/

template<typename Delegate>
class MessagePublisher
{
public:
    using function_type = Delegate;
    
    ~MessagePublisher();

    /// Constructs a dispatcher, preallocating buffers used for delegating
    /// messages.
    ///
    /// @param  delegateCount Estimated number of concurrently registered
    ///                       delegates
    ///
    MessagePublisher
    (
        MessageStream&& stream,
        size_t delegateCount=32,
        const Allocator& allocator=Allocator()
    );
    /// The message delegate
    
    ///
    /// Queues a message for dispatch as an event.
    ///
    /// @param  msg      The message object to stream
    /// @param  msgClass The Message Class
    /// @param  targetId An ID representing the target of a message.  If zero,
    ///                  then the event is considered global.  Otherwise, the
    ///                  event is sent to all handlers with the specified target
    ///
    void queue
    (
        ::capnp::MessageBuilder& msg,
        const MessageHeader::ClassId& msgClass,
        MessageHeader::SequenceId targetId=0
    );
    
    ///
    /// Queues a message for dispatch as a command.
    ///
    /// @param  msg      The message object to stream
    /// @param  msgClass The Message Class
    /// @param  cb       (Optional) Callback issued upon command dispatch.
    /// @return A handle used for cancelling delegate dispatch
    ///
    MessageDelegateHandle queueCommand
    (
        ::capnp::MessageBuilder& msg,
        const MessageHeader::ClassId& msgClass,
        const function_type& cb=Delegate()
    );
    
    ///
    /// Queues a message for dispatch as a command response.  If there is no
    /// Command Delegate linked to the supplied sequence ID, then the message
    /// is NOT queued.
    ///
    /// @param  msg      The message object to stream
    /// @param  msgClass The Message Class
    /// @param  cmdId    The Sequence ID as passed by the incoming command
    ///
    void queueResponse
    (
        ::capnp::MessageBuilder& msg,
        const MessageHeader::ClassId& msgClass,
        MessageHeader::SequenceId cmdId
    );
    
    ///
    /// Subscribe to a specific message type
    /// @param  msgClass The Message Class
    /// @param  del      The delegate executed when a message of class 'id' is
    ///                  processed
    /// @param  targetId A target filter, used to register for targeted events.
    ///                  A zero target means that the handler will intercept all
    ///                  events of the specified msgClass.
    /// @return A handle to the subscribed delegate.  Subscribers must
    ///         unsubscribe at some point if the subscriber id destroyed before
    ///         the MessageDispatcher
    ///
    MessageDelegateHandle on
    (
        const MessageHeader::ClassId& msgClass,
        const Delegate& del,
        MessageHeader::SequenceId targetId=0
    );
    ///
    /// Unsubscribes a delegate.  This must occur before the delegate owner is
    /// destroyed.
    ///
    /// @param delegateHandle The delegate object handle.
    ///
    void remove(MessageDelegateHandle delegateHandle);
    
private:
    template<typename Publisher>
    friend MessageStream setMessageStreamForPublisher(
            MessageStream&& publishTargetStream,
            Publisher& publisher);
    
    template<typename Publisher>
    friend void dispatchMessageStreamToPublisher(MessageStream& stream,
        Publisher& publisher);

private:
    MessageStream acquireStream(MessageStream&& publishTargetStream);
    void dispatch(MessageStream& messages);
    
    
    MessageStream _msgStream;
    MessageHeader::SequenceId _nextSeqId;
    MessageDelegateHandle _nextDelegateHandle;

    // Store for Delegates by their delegate handle
    struct DelegateEntry
    {
        function_type del;
        DelegateEntry* next = nullptr;
        DelegateEntry* prev = nullptr;
        MessageDelegateHandle handle = 0;          // ref to its owner
        MessageHeader::SequenceId seqId = 0;
        int32_t refcnt = 0;
    };
    ObjectPool<DelegateEntry> _delegatePool;
    unordered_map<MessageDelegateHandle, DelegateEntry*> _delegates;

    //  handles the case where a delegate deregisters itself - refcounting
    void obtainDelegateEntry(DelegateEntry* entry);
    void releaseDelegateEntry(DelegateEntry* entry);

    // These maps contain references to DelegateEntry objects from the
    // _delegates and _delegatePool containers.
    // The _classDelegates vector contains a head node of a delegate list
    // where each sibling node resides within the _delegates array
    vector<std::pair<MessageHeader::ClassId, DelegateEntry*>> _classDelegates;
    unordered_map<MessageHeader::SequenceId, DelegateEntry*> _sequenceDelegates;

    template<typename Key, typename Container>
    auto lowerBoundIterator(const Key& key, Container& container) ->
        typename Container::iterator
    {
        return std::lower_bound(container.begin(), container.end(),
                                key,
                               [](const typename Container::value_type &l,
                                  const Key& r) -> bool
                               {
                                   return l.first < r;
                               });
    }
};

template<typename Delegate>
MessagePublisher<Delegate>::MessagePublisher
(
    MessageStream&& stream,
    size_t delegateCount,
    const Allocator& allocator
) :
    _msgStream(std::move(stream)),
    _nextSeqId(0),
    _nextDelegateHandle(0),
    _delegatePool(delegateCount*2, allocator),
    _delegates(allocator),
    _classDelegates(allocator),
    _sequenceDelegates(allocator)
{
    _delegates.reserve(delegateCount);
    _classDelegates.reserve(delegateCount/2);
    _sequenceDelegates.reserve(delegateCount/2);
}

template<typename Delegate>
MessagePublisher<Delegate>::~MessagePublisher()
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

template<typename Delegate>
void MessagePublisher<Delegate>::queue
(
    ::capnp::MessageBuilder& msg,
    const MessageHeader::ClassId& msgClass,
    MessageHeader::SequenceId targetId
)
{
    MessageHeader header;
    
    header.type = MessageHeader::kEvent;
    header.seqId = targetId;
    header.classId = msgClass;

    writeMessageHeader(_msgStream, header);
    writeMessage(_msgStream, msg);
}

template<typename Delegate>
MessageDelegateHandle MessagePublisher<Delegate>::queueCommand
(
    ::capnp::MessageBuilder& msg,
    const MessageHeader::ClassId& msgClass,
    const Delegate& cb
)
{
    ++_nextSeqId;
    if (!_nextSeqId)
        _nextSeqId = 1;

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
    
    header.type = MessageHeader::kCommand;
    header.seqId = _nextSeqId;
    header.classId = msgClass;

    writeMessageHeader(_msgStream, header);
    writeMessage(_msgStream, msg);
    
    return cb ? _nextDelegateHandle : 0;
}

template<typename Delegate>
void MessagePublisher<Delegate>::queueResponse
(
    ::capnp::MessageBuilder& msg,
    const MessageHeader::ClassId& msgClass,
    MessageHeader::SequenceId cmdId
)
{
    if (_sequenceDelegates.find(cmdId) == _sequenceDelegates.end())
    {
        CK_LOG_WARN("MessageDispatcher", "No sequence delegate found for "
                                         "Command ID %u", cmdId);
        return;
    }
    
    MessageHeader header;
    
    header.classId = msgClass;
    header.type = MessageHeader::kResponse;
    header.seqId = cmdId;
    
    writeMessageHeader(_msgStream, header);
    writeMessage(_msgStream, msg);
}

template<typename Delegate>
MessageDelegateHandle MessagePublisher<Delegate>::on
(
    const MessageHeader::ClassId& id,
    const Delegate& del,
    MessageHeader::SequenceId targetId
)
{
    auto it = lowerBoundIterator(id, _classDelegates);

    //  add an anchor-style head list node for the class
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
    entry->seqId = targetId;
    entry->next = it->second;
    entry->prev = it->second->prev;
    entry->prev->next = entry;
    entry->next->prev = entry;

    return _nextDelegateHandle;
}

template<typename Delegate>
void MessagePublisher<Delegate>::remove(MessageDelegateHandle delegateHandle)
{
    auto it = _delegates.find(delegateHandle);
    if (it == _delegates.end())
        return;

    auto entry = it->second;
    //entry->handle = 0;
    //_delegates.erase(it);

    //  releasing the delegate will clear it automatically from the
    //  classDelegates delegate list (while maintaining the class entry in the
    //  table for optimization/reuse).
    releaseDelegateEntry(entry);
}

template<typename Delegate>
MessageStream MessagePublisher<Delegate>::acquireStream(MessageStream&& publishTargetStream)
{
    MessageStream oldStream = std::move(_msgStream);
    _msgStream = std::move(publishTargetStream);
    return oldStream;
}

template<typename Delegate>
void MessagePublisher<Delegate>::dispatch(MessageStream& stream)
{
    MessageHeader hdr;
    do
    {
        readMessageHeader(stream, hdr);
        if (hdr.type <= MessageHeader::kEnd)
            continue;
   
        capnp::InputStreamMessageReader reader(stream);
        
        if (hdr.type == MessageHeader::kResponse)
        {
            if (hdr.seqId)
            {
                auto it = _sequenceDelegates.find(hdr.seqId);
                if (it != _sequenceDelegates.end())
                {
                    auto entry = it->second;
                    {
                        obtainDelegateEntry(entry);
                        entry->del(hdr.classId, reader, entry->seqId);
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
        }
        else
        {
            //  dispatch to any class-based listeners
            auto it = lowerBoundIterator(hdr.classId, _classDelegates);
            if (it != _classDelegates.end() && it->first == hdr.classId)
            {
                auto head = it->second;
                auto entry = head->next;
                while (entry != head)
                {
                    auto next = entry->next;

                    //  Filter Event Messages based on target (seqId) zero
                    //  target delegates always dispatch otherwise the target
                    //  must match.
                    //
                    //  Command Messages are always processed.
                    //
                    if (hdr.type != MessageHeader::kEvent ||
                        !entry->seqId ||
                        entry->seqId == hdr.seqId)
                    {
                        obtainDelegateEntry(entry);

                        entry->del(hdr.classId, reader, hdr.seqId);

                        releaseDelegateEntry(entry);
                    }
                    entry = next;
                }
            }
        }
    }
    while(hdr.type > MessageHeader::kEnd);
    
    stream.reset();
}

template<typename Delegate>
void MessagePublisher<Delegate>::obtainDelegateEntry(DelegateEntry* entry)
{
    ++entry->refcnt;
}

template<typename Delegate>
void MessagePublisher<Delegate>::releaseDelegateEntry(DelegateEntry* entry)
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

#endif
