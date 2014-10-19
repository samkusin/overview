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

namespace cinek {
    using namespace overview;
}

namespace cinek { namespace overview {

unique_ptr<Simulation> generateSimulation(const GameTemplates& gameTemplates,
                                          const CreateSimulationParams& createParams)
{
    Allocator allocator = createParams.allocator;

    SimulationParams params;
    params.entityLimit = 32;
    params.allocator = allocator;

    auto simulation = allocate_unique<Simulation>(allocator,
                                                  gameTemplates,
                                                  params);
    return std::move(simulation);
}

} /* namespace overview */ } /* namespace cinek */
