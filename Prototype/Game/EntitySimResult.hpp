//
//  EntitySimResult.hpp
//  Overview
//
//  Created by Samir Sinha on 11/8/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_SimResult_Entity_hpp
#define Overview_Game_SimResult_Entity_hpp

#include "Core/EventBase.hpp"
#include "Game/EntityTypes.hpp"

namespace cinek { namespace overview {

class EntitySimResult : public EventBase
{
public:
    static const UUID kClassID;
    
    const UUID& classID() const override {
        return kClassID;
    }
    
public:
    EntitySimResult(EntityId id, uint32_t flags, uint32_t timeMs) :
        EventBase(timeMs),
        _id(id),
        _flags(flags) {}
    
    EntityId id() const { return _id; }
    uint32_t flags() const { return _flags; }
    
private:
    EntityId _id;
    uint32_t _flags;
};

} /* namespace overview */ } /* namespace cinek */

#endif