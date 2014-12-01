//
//  MoveEntityEvent.hpp
//  Overview
//
//  Created by Samir Sinha on 11/29/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_MoveEntity_Event_hpp
#define Overview_Game_MoveEntity_Event_hpp

#include "Game/SimulationTypes.hpp"
#include "Core/SDO/SDO.hpp"

namespace cinek { namespace overview {

class MoveEntityEvent : public SDO
{
public:
    CINEK_SDO_CLASS(MoveEntityEvent)

    MoveEntityEvent() = default;

    void setEntityId(EntityId id) { _entityId = id; }
    EntityId entityId() const { return _entityId; }
    
    void setTransform(const WorldObjectTransform& transform) { _transform = transform; }
    const WorldObjectTransform& transform() const { return _transform; }
    
private:
    EntityId _entityId;
    WorldObjectTransform _transform;
};


} /* namespace overview */ } /* namespace cinek */

#endif 
