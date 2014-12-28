//
//  WorldMap.cpp
//  Overview
//
//  Created by Samir Sinha on 12/24/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Game/WorldMap.hpp"
#include "Shared/StaticWorldMap.hpp"

namespace cinek { namespace overview {

WorldMap::WorldMap(const StaticWorldMap& worldMap) :
    WorldObjectBase(ClassId::kWorldMap),
    _worldMap(worldMap)
{
}

AABB<Point> WorldMap::bounds() const
{
    return _worldMap.bounds();
}
    
} /* namespace overview */ } /* namespace cinek */
