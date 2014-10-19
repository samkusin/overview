//
//  Simulation.cpp
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Game/Simulation.hpp"
#include "Game/World.hpp"
#include "Shared/GameTemplates.hpp"
#include "Engine/Model/TileGridMap.hpp"

namespace cinek {
    using namespace overview;
}

namespace cinek { namespace overview {

Simulation::Simulation(const overview::GameTemplates& gameTemplates,
                       const SimulationParams& params) :
        _allocator(params.allocator),
        _gameTemplates(gameTemplates),
        _entityPool(params.entityLimit, _allocator)
{
    auto tileDims = gameTemplates.tileGridMap()->overlayDimensions();
    auto tileHeight = gameTemplates.tileGridMap()->overlayToFloorRatio();

    WorldAABB worldBounds;
    worldBounds.min = WorldPoint(0,0,0);
    worldBounds.max = WorldPoint(tileDims.x, tileDims.y, tileHeight);

    _world = allocate_unique<World>(_allocator,
                                    worldBounds,
                                    _allocator);
}

Simulation::~Simulation()
{

}

} /* namespace overview */ } /* namespace cinek */
