//
//  Simulation.hpp
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_Simulation_hpp
#define Overview_Game_Simulation_hpp

#include "Game/WorldTypes.hpp"
#include "Game/Entity.hpp"

#include "cinek/objectpool.hpp"
#include "cinek/allocator.hpp"


namespace cinekine { namespace ovengine {
    class GameTemplates;
    class World;
} /* namespace ovengine */ } /* namespace cinekine */

namespace cinekine { namespace prototype {
    
    class Simulation
    {
    public:
        Simulation(ObjectPool<ovengine::Entity>&& entityPool,
                   unique_ptr<ovengine::World>&& worldPtr,
                   const ovengine::GameTemplates& gameTemplates,
                   const Allocator& allocator);
        ~Simulation();
        
        void createEntity(const std::string& templateName);
        
    private:
        Allocator _allocator;
        const ovengine::GameTemplates& _gameTemplates;
        ObjectPool<ovengine::Entity> _entityPool;
        unique_ptr<ovengine::World> _world;
    };
    
} /* namespace ovengine */ } /* namespace cinekine */

#endif /* defined(__Overview__Simulation__) */
