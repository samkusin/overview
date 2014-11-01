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

#include "SDO/Game/CreateEntity.hpp"

namespace cinek {
    using namespace overview;
}

namespace cinek { namespace overview {

Simulation::Simulation(
    const overview::GameTemplates& gameTemplates,
    const SimulationParams& params
) :
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

void Simulation::createEntity(
    const CreateEntityRequest& request,
    const std::function<void(const CreateEntityResponse&)>& cb
)
{
    // ListenerHandle <- Schedule a listener for a CreateEntity event
    //
    // Create and schedule a CreateEntityTask with listenerHandle
    //  * The task will publish an event upon completion to the sim queue
    //    with a ListenerHandle.
    //  * The Event system will issue the Listener callback associated
    //    with the unique ListenerHandle.
    //    - The ref to the Response Data object must be contained within
    //      the Event
    //
    // We need an intermediary functor object here to cast the Event
    // listener response callback to the supplied createEntity callback
    // in this method (instead of std::function for efficiency reasons?)

}

} /* namespace overview */ } /* namespace cinek */
