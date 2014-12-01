//
//  MoveEntityRequest.hpp
//  Overview
//
//  Created by Samir Sinha on 11/29/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_MoveEntityRequest_hpp
#define Overview_Game_MoveEntityRequest_hpp

#include "Game/SimulationTypes.hpp"
#include "Core/SDO/SDO.hpp"

namespace cinek { namespace overview {

class MoveEntityRequest : public SDO
{
public:
    CINEK_SDO_CLASS(MoveEntityRequest)
    
public:
    MoveEntityRequest() = default;
    
    void setEntityId(EntityId entityId) { _entityId = entityId; }
    EntityId entityId() const { return _entityId; }
    
    const WorldObjectTransform& transform() const { return _transform; }
    void setTransform(const WorldObjectTransform& transform) { _transform = transform; }
    
private:
    EntityId _entityId;
    WorldObjectTransform _transform;
};


} /* namespace overview */ } /* namespace cinek */

#endif
