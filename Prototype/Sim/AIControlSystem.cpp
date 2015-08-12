//
//  AIControlSystem.cpp
//  Overview
//
//  Created by Samir Sinha on 8/4/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "AIControlSystem.hpp"
#include "Engine/MessageDispatcher.hpp"

namespace cinek { namespace ovproto {


AIState::AIState
(
    AIControlSystem& aiControl,
    size_t streamSizeBytes,
    uint32_t numObjects,
    const Allocator& allocator
) :
    _owner(&aiControl),
    _timeInSecs(0),
    _msgStreamNext(streamSizeBytes, allocator),
    _msgPublisher(std::move(overview::MessageStream(streamSizeBytes, allocator)),
        2048,
        allocator),
    _delegateHandles(allocator)
{
    _delegateHandles.reserve(numObjects*4);
}

void AIState::onAIObjectEnd(AIObject& obj)
{
    //  cancel all remaining delegates on the object
    MessageDelegateData comp;
    comp.objId = obj.id();
    auto range = std::equal_range(_delegateHandles.begin(), _delegateHandles.end(), comp);
    for (auto it = range.first; it != range.second; ++it)
    {
        _msgPublisher.remove(it->handle);
    }
    _delegateHandles.erase(range.first, range.second);
}

void AIState::MessageDelegate::operator()
(
    const ::cinek::overview::MessageHeader::ClassId& classId,
    ::capnp::MessageReader& msg,
    const ::cinek::overview::MessageHeader::SequenceId seqId
)
{
    AIObject* obj = ais->_owner->findObject(objId);
    if (!obj)
    {
        CK_LOG_ERROR("AIState", "AIObject[%u] delegates should have been cleared ",
                     objId);
        return;
    }
    delegate(*ais, *obj, classId, msg);
}

void AIState::queueCommand
(
    AIObjectId objId,
    ::capnp::MessageBuilder& msg,
    const ::cinek::overview::MessageHeader::ClassId& msgClass,
    const Delegate& delegate
)
{
    MessageDelegate msgDel;
    msgDel.objId = objId;
    msgDel.ais = this;
    msgDel.delegate = delegate;
    _msgPublisher.queueCommand(msg, msgClass, msgDel);
}

void AIState::notifyHandler
(
    AIObjectId objId,
    const ::cinek::overview::MessageHeader::ClassId& msgClass,
    const Delegate& delegate
)
{
    MessageDelegate msgDel;
    msgDel.objId = objId;
    msgDel.ais = this;
    msgDel.delegate = delegate;
    _msgPublisher.on(msgClass, msgDel);
}

void AIState::dispatchMessages()
{
    auto messagesForDispatch = overview::setMessageStreamForPublisher(
        std::move(_msgStreamNext),
        _msgPublisher
    );
    
    overview::dispatchMessageStreamToPublisher(messagesForDispatch, _msgPublisher);

    _msgStreamNext = std::move(messagesForDispatch);
}
    
    
////////////////////////////////////////////////////////////////////////////////

AIControlSystem::AIControlSystem
(
    const InitParams& params,
    const Allocator& allocator
) :
    _lastGCTimeInSecs(-1),
    _inactiveCount(0),
    _objects(allocator),
    _nextSimObjectIndex(0),
    _pendingSimObjectCount(0),
    _objectsPerSecond(params.objectsPerSecond)
{
    Allocator localAllocator(allocator);
    _objects.reserve(params.objectCount);
    
    _state = allocate_unique<AIState>(localAllocator,
        *this,
        params.msgStreamSizeInBytes,
        params.objectCount,
        allocator
    );
}

const AIObject* AIControlSystem::findObject(AIObjectId id) const
{
    auto it = std::lower_bound(_objects.begin(), _objects.end(), id,
        [](const AIObject& obj, const AIObjectId& id) -> bool {
            return obj.id() < id;
        });
    if (it == _objects.end() || it->id() != id || it->state() == AIObject::State::kInactive)
        return nullptr;
    
    return &(*it);
}

AIObject* AIControlSystem::findObject(AIObjectId id)
{
    return const_cast<AIObject*>(static_cast<const AIControlSystem*>(this)->findObject(id));
}


void AIControlSystem::addObject(AIObject&& object)
{
    AIObjectId nextObjId;
    AIObject* emplacedObject = nullptr;
    
    if (_objects.size() < _objects.capacity())
    {
        //  Take advantage of being a sorted vector.
        //  The last object is the highest object id.
        //  Thus adding an object is trivial (+1 the ID of the last object and
        //  add the new object to the tail.
        nextObjId = !_objects.empty() ? _objects.back().id()+1 : 1;
        _objects.emplace_back(object);
        emplacedObject = &_objects.back();
    }
    else
    {
        //  find a free object id if available
        //  if not, then we fail.
        //  note this method should be seldom if ever called since our garbage
        //  collection method should compress the objects vector
        CK_ASSERT(false);
        
        nextObjId = 0;
        for (auto& obj : _objects)
        {
            if (obj.state() == AIObject::State::kInactive)
            {
                nextObjId = obj.id();
                obj = std::move(object);
                emplacedObject = &obj;
                break;
            }
        }
    }
    
    CK_ASSERT_RETURN(nextObjId > 0 && emplacedObject);    // very unlikely
    
    emplacedObject->setId(nextObjId);
    emplacedObject->setState(AIObject::State::kBegin);
}

void AIControlSystem::gc(double timeInSecs)
{
    // TODO!
}

} /* namespace ovproto */ } /* namespace cinek */
