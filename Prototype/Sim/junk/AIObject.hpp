//
//  AIObject.hpp
//  Overview
//
//  Created by Samir Sinha on 8/5/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Sim_AIObject_hpp
#define Overview_Sim_AIObject_hpp

#include "GameTypes.hpp"

namespace cinek { namespace ovproto {

class AIControlSystem;
class AIState;

//  For each AIObject
//  Code accesses AIObject for basic state
//  Code accesses additional data like graphs, trees, component data
//  Code pushes commands onto a MessageStream and registers a handler for the
//      response.

using AIObjectId = ::cinek::overview::MessageHeader::SequenceId;

struct AIObject
{
    AIObject() : _id(0), _state(State::kInactive) {}
    AIObject(AIObjectId id, Entity entity) : _id(id), _entity(entity) {}

    AIObjectId id() const { return _id; }
    Entity entity() const { return _entity; }

private:
    void clearEntity() { _entity = nullptr; }
    void clearObject() { _id = 0; }
    void setId(AIObjectId id) { _id = id; }

    enum class State
    {
        kInactive,      // uninitialized
        kBegin,         // begin AI state
        kActive,        // active simulation state
        kSuspend,       // AI going into suspend state
        kIdle,          // Suspended
        kResume,        // AI going into resume state
        kEnd            // AI going into finish state
    };
    
    State state() const { return _state; }
    void setState(State state) { _state = state; }

private:
    friend class AIControlSystem;
    
    // Used by the messaging system for sending messages targeted to this AI
    // This ID is assigned upon registering a new AIObject with the
    // AIControlSystem
    AIObjectId _id;
    
    // Identifies the Entity linked to this AIObject
    Entity _entity;
    
    // Used by AIControlSystem::simulate to
    State _state;
};

} /* namespace ovproto */ } /* namespace cinek */

#endif
