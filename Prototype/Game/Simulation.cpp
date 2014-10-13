//
//  Simulation.cpp
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Game/Simulation.hpp"
#include "Game/Model/World.hpp"
#include "Engine/Model/TileGridMap.hpp"


namespace cinekine { namespace ovengine {
    
Simulation::Simulation(const TileGridMap& gridMap,
                       const Allocator& allocator) :
    _allocator(allocator),
    _tileGridMap(gridMap),
    _entityPool(256, _allocator)
{
    auto tileDims = _tileGridMap.overlayDimensions();
    auto height = _tileGridMap.overlayToFloorRatio();   // 1 tile for now
    
    AABB<Point> worldBounds;
    worldBounds.min = Point(0,0,0);
    worldBounds.max = Point(tileDims.x, tileDims.y, height);
    _world = unique_ptr<World>(_allocator.newItem<World>(worldBounds, _allocator),
                               _allocator);
}
    
Simulation::~Simulation()
{
    
}
    
} /* namespace ovengine */ } /* namespace cinekine */
