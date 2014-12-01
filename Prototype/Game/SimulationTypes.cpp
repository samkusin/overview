//
//  SimulationTypes.cpp
//  Overview
//
//  Created by Samir Sinha on 11/30/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Game/SimulationTypes.hpp"

namespace cinek { namespace overview {

    
WorldObjectTransform& WorldObjectTransform::operator|=(
    const WorldObjectTransform& transform
)
{
    if (transform.isDirty(WorldObjectTransform::kDirection))
    {
        dir = transform.dir;
        setDirty(WorldObjectTransform::kDirection);
    }
    if (transform.isDirty(WorldObjectTransform::kPosition))
    {
        pos = transform.pos;
        setDirty(WorldObjectTransform::kPosition);
    }
    if (transform.isDirty(WorldObjectTransform::kSpeed))
    {
        speed = transform.speed;
        setDirty(WorldObjectTransform::kSpeed);
    }
    return *this;
}
    

} /* namespace overview */ } /* namespace cinek */
