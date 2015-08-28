//
//  ActionTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 8/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Sim_ActionTypes_hpp
#define Overview_Sim_ActionTypes_hpp

#include "GameTypes.hpp"

namespace cinek { namespace ovproto {

using ActionId = uint32_t;

//  <action type> identifies what action class this handle belongs to.
//  <action id> is the Simulation<Action> key for the action
//  Combined they are used to direct requests to a set of Simulation<Action>
//  containers.
//
using ActionHandle = std::pair<uint32_t, uint32_t>;

struct ActionObject
{
    enum Type
    {
        kUndefined,
        kEntity
    };

    //  object
    Type type;

    //  complement
    union
    {
        Entity entity;
    };
};

struct ActionPhrase
{
    ActionObject origin;
    uint32_t action;
    ActionObject object;
    ActionObject target;
};

class Action
{
public:
    using return_type = uint32_t;
    using key_type = ActionId;
    
    Entity owner() const             { return _id; }
    void setOwner(Entity id)         { _id = id; }
    
private:
    Entity _id;
};


} /* namespace ovproto */ } /* namespace cinek */


#endif
