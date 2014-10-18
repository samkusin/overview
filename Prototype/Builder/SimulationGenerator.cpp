//
//  SimulationGenerator.cpp
//  Overview
//
//  Created by Samir Sinha on 10/17/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Builder/SimulationGenerator.hpp"
#include "Game/Simulation.hpp"
#include "Game/World.hpp"
#include "Game/Entity.hpp"
#include "Shared/GameTemplates.hpp"

#include "Engine/Model/TileGridMap.hpp"

namespace cinekine {
    using namespace ovengine;
}

namespace cinekine { namespace prototype {
    
unique_ptr<Simulation> generateSimulation(const GameTemplates& gameTemplates,
                                          const CreateSimulationParams& createParams)
{
    Allocator allocator = createParams.allocator;
    ObjectPool<Entity> entityPool(createParams.entityLimit, allocator);
    
    auto tileDims = gameTemplates.tileGridMap()->overlayDimensions();
    auto tileHeight = gameTemplates.tileGridMap()->overlayToFloorRatio();
    
    WorldAABB worldBounds;
    worldBounds.min = WorldPoint(0,0,0);
    worldBounds.max = WorldPoint(tileDims.x, tileDims.y, tileHeight);
    
    auto world = allocate_unique<World>(allocator,
                                        worldBounds,
                                        allocator);
    
    auto simulation = allocate_unique<Simulation>(allocator,
                                                  std::move(entityPool),
                                                  std::move(world),
                                                  gameTemplates,
                                                  allocator);
    return std::move(simulation);
}
    
} /* namespace prototype */ } /* namespace cinekine */
