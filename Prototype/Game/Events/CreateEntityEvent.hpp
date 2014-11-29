//
//  CreateEntityEvent.h
//  Overview
//
//  Created by Samir Sinha on 11/28/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_CreateEntityEvent_hpp
#define Overview_Game_CreateEntityEvent_hpp

#include "Game/SimulationTypes.hpp"
#include "Core/SDO/SDO.hpp"

namespace cinek { namespace overview {

class CreateEntityEvent : public SDO
{
public:
    CINEK_SDO_CLASS(CreateEntityEvent)

    CreateEntityEvent() = default;

    void setEntityId(EntityId id) { _entityId = id; }
    EntityId entityId() const { return _entityId; }
    
private:
    EntityId _entityId;
};


} /* namespace overview */ } /* namespace cinek */

#endif 
