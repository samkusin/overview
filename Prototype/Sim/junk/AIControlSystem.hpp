//
//  AIControlSystem.hpp
//  Overview
//
//  Created by Samir Sinha on 8/4/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Sim_AIControlSystem_hpp
#define Overview_Sim_AIControlSystem_hpp

#include "AIObject.hpp"

#include "Engine/MessageStream.hpp"
#include "Engine/MessagePublisher.hpp"

#include <cinek/vector.hpp>
#include <cinek/map.hpp>

namespace cinek { namespace ovproto {

//  A system of AIObjects (AISystem?)
//  A vector of AIObjects
//  Code dispatches the command MessageStream.
//  Code parses a MessageStream for responses and notfications that may add
//      additional messages to a MessageStream
//  Code runs simulation on a range of AIObjects
//  Code periodically cleans up dead AIObjects
//  This may involve compression of the AIObjects vector for data locality
//  AIObjects are sorted into the vector (By EntityId)
//      If there is no room, then compress the vector
//      If there is no room after this, then we have hit a limit
//      The game should allocate enough AIObjects to prevent this condition
//      The game should compress the AIObject vector with enough regularity to
//          prevent the need for on-demand compression (which may occur at an
//          inconvient time for the simulation (CPU cache, frame rate hits)

//  At no time will other systems reference AIObjects by pointer or index

//  Operations:
//      Update(AIObject, MessagePublisher, WorldState);
//      Swap MessageStream, MessagePublisher.MessageStream'
//      Dispatch(MessageStream, MessagePublisher);
//

//  Example:
//      AIContext AIS;
//      vector<AIObject> objects;
//      std::for_each(objects.begin(), objects.end(),
//          [&AIS](AIObject& object)
//          {
//              AIS.queueCommand(..., Delegate);
//              AIS.notifyHandler(MsgClassId, Delegate);
//              ...
//          });
//

//  Notes:
//      AIObject data does not need to directly refer to other AIObjects.
//      AI Functionality access of other AI data occurs through looking up
//          components and other world state information (AIObject data is only
//          read and written by the function accessing it.
//      *Delegate* is defined by the AIContext/AISystem.  Delegate will contain
//          a reference to the target's AIObject.  This is to prevent AIObjects
//          from accessing other AIObjects.
//

class AIState
{
public:
    using Delegate = void (*)(AIState& system, AIObject& aiObject,
                                const ::cinek::overview::MessageHeader::ClassId&,
                                ::capnp::MessageReader& msg);
 
    void queueCommand(AIObjectId objId,
                      ::capnp::MessageBuilder& msg,
                      const ::cinek::overview::MessageHeader::ClassId& msgClass,
                      const Delegate& delegate = Delegate());
    
    void notifyHandler(AIObjectId objId,
                       const ::cinek::overview::MessageHeader::ClassId& msgClass,
                       const Delegate& delegate);
    
    double currentTimeInSeconds() const { return _timeInSecs; }
    
private:
    AIState(AIControlSystem& aiControl, size_t streamSizeBytes,
        uint32_t numObjects,
        const Allocator& allocator);
    
    void setTime(double timeInSecs) { _timeInSecs = timeInSecs; }
    
    void onAIObjectEnd(AIObject& obj);
    
    void dispatchMessages();

private:
    friend class AIControlSystem;
    friend class ::cinek::Allocator;
    
    AIControlSystem* _owner;
    double _timeInSecs;
    
    struct MessageDelegate
    {
        AIObjectId objId;
        AIState* ais;
        Delegate delegate;
        MessageDelegate(): delegate(nullptr) {}
        operator bool() const { return delegate != nullptr; }
        void operator()(const ::cinek::overview::MessageHeader::ClassId&,
                           ::capnp::MessageReader&,
                           ::cinek::overview::MessageHeader::SequenceId);
    };

    ::cinek::overview::MessageStream _msgStreamNext;
    ::cinek::overview::MessagePublisher<MessageDelegate> _msgPublisher;
    
    struct MessageDelegateData
    {
        AIObjectId objId;       // sort key
        ::cinek::overview::MessageDelegateHandle handle;
        
        bool operator<(const MessageDelegateData& other) const {
            return objId < other.objId;
        }
    };
    
    vector<MessageDelegateData> _delegateHandles;
};


class AIControlSystem
{
public:
    struct InitParams
    {
        size_t msgStreamSizeInBytes;
        uint32_t objectCount;
        int objectsPerSecond;
    };

    AIControlSystem(const InitParams& params, const Allocator& allocator=Allocator());

    // Adds the initialized object into the simulation
    //
    void addObject(AIObject&& object);
    
    //  AIClient must have the following implementation
    //      void begin(AIState& state, AIObject& object);
    //      void simulate(AIState& state, AIObject& object);
    //      void end(AIState& state, AIObject& object);
    //      void suspend(AIState& state, AIObject& object);
    //      void resume(AIState& state, AIObject& object);
    //
    template<typename AIClient>
    void simulate(AIClient& client, double timeInSecs);
    
    // O(log N)
    const AIObject* findObject(AIObjectId id) const;
    AIObject* findObject(AIObjectId id);
    
private:
    void gc(double timeInSecs);
    
private:
    unique_ptr<AIState> _state;

    //  last time garbage collection (cleanup phase) was executed
    //  used to determine whether the system should execute a GC pass at the
    //  end of the simulate frame.
    double _lastGCTimeInSecs;
    uint32_t _inactiveCount;

    //  Sorted by AIObjectId.   The object array may have 'dead ranges' where
    //  dead AIObjects (with null entities) remain until the next garbage
    //  collection pass.
    vector<AIObject> _objects;
    uint32_t _nextSimObjectIndex;
    double _pendingSimObjectCount;
    int _objectsPerSecond;
};

template<typename AIClient>
void AIControlSystem::simulate
(
    AIClient& client,
    double timeInSecs
)
{
    auto intervalInSecs = timeInSecs - _state->currentTimeInSeconds();
    
    _state->setTime(timeInSecs);
    _state->dispatchMessages();
    
    if (!_objects.empty())
    {
        //  we need to persist any fractional counts for an accurate objs/second
        //  simulation.
        _pendingSimObjectCount += intervalInSecs * _objectsPerSecond;
        
        int maxObjsSimulated = (int)(_pendingSimObjectCount);
        int numObjsSimulated = 0;
        uint32_t firstSimObjectIndex = _nextSimObjectIndex;
        
        while (numObjsSimulated < maxObjsSimulated)
        {
            auto& obj = _objects[_nextSimObjectIndex];
            if (obj.state() != AIObject::State::kInactive &&
                obj.state() != AIObject::State::kIdle)
            {
                switch (obj.state())
                {
                case AIObject::State::kBegin:
                    client.begin(*_state.get(), obj);
                    break;
                case AIObject::State::kActive:
                    client.end(*_state.get(), obj);
                    break;
                case AIObject::State::kSuspend:
                    client.suspend(*_state.get(), obj);
                    break;
                case AIObject::State::kResume:
                    client.resume(*_state.get(), obj);
                    break;
                case AIObject::State::kEnd:
                    client.end(*_state.get(), obj);
                    _state->onAIObjectEnd(obj);
                    break;
                default:
                    break;
                }
                ++numObjsSimulated;
            }
            _nextSimObjectIndex = (_nextSimObjectIndex+1) % _objects.size();
            if (_nextSimObjectIndex == firstSimObjectIndex)
                break;
        }
        
        _pendingSimObjectCount -= maxObjsSimulated; // persists fractional amts
    }
    
    gc(timeInSecs);
}

} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_Sim_AIControlSystem_hpp) */
