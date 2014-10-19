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

#include "Gen/Entity.pb.h"


namespace cinek { namespace overview {
    class GameTemplates;
    class World;
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

        //void addPlayer(const AddPlayerRequest& req, );

    private:
        Allocator _allocator;
        const overview::GameTemplates& _gameTemplates;
        ObjectPool<overview::Entity> _entityPool;
        unique_ptr<overview::World> _world;
    };

} /* namespace overview */ } /* namespace cinek */

#endif /* defined(__Overview__Simulation__) */
