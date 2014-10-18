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

namespace cinekine {
    using namespace ovengine;
}

namespace cinekine { namespace prototype {
    
    Simulation::Simulation(ObjectPool<ovengine::Entity>&& entityPool,
                           unique_ptr<ovengine::World>&& worldPtr,
                           const ovengine::GameTemplates& gameTemplates,
                           const Allocator& allocator) :
    _allocator(allocator),
    _gameTemplates(gameTemplates),
    _entityPool(std::move(entityPool)),
    _world(std::move(worldPtr))
{
}
    
Simulation::~Simulation()
{
    
}
    
} /* namespace ovengine */ } /* namespace cinekine */
