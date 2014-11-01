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

#include <functional>


namespace cinek { namespace overview {
    class GameTemplates;
    class World;
    
    class CreateEntityRequest;
    class CreateEntityResponse;

} /* namespace overview */ } /* namespace cinek */

namespace cinek { namespace overview {

    struct SimulationParams
    {
        uint32_t entityLimit;
        Allocator allocator;
    };

    class Simulation
    {
    public:
        Simulation(const overview::GameTemplates& gameTemplates,
                   const SimulationParams& params);
        ~Simulation();

        /// Creates an Entity from a named template.  The response contains
        /// the created entity by template and its unique (per simulation) ID
        /// value.
        ///
        /// @param  cb      Resultant entity
        /// @param  request Request with the entity definition used to
        ///                       create the instance.
        /// @return The command result
        void createEntity(const CreateEntityRequest& request,
                          const std::function<void(const CreateEntityResponse&)>& cb);
        
        /// Steps the simulation.
        ///
        /// @param The current simulation time.  
        void update(uint32_t frameTimeMs);
        
    private:
        Allocator _allocator;
        const overview::GameTemplates& _gameTemplates;
        ObjectPool<overview::Entity> _entityPool;
        unique_ptr<overview::World> _world;
    };

} /* namespace overview */ } /* namespace cinek */

#endif /* defined(__Overview__Simulation__) */
