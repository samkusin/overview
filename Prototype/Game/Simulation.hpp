//
//  Simulation.hpp
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_Simulation_hpp
#define Overview_Game_Simulation_hpp

#include "Game/Model/WorldTypes.hpp"
#include "Game/Model/Entity.hpp"

#include "cinek/objectpool.hpp"
#include "cinek/allocator.hpp"


namespace cinekine { namespace ovengine {
    class TileGridMap;
    class World;
} /* namespace ovengine */ } /* namespace cinekine */

namespace cinekine { namespace ovengine {
    
    class Simulation
    {
    public:
        Simulation(const TileGridMap& gridMap,
                   const Allocator& allocator);
        ~Simulation();
        
    private:
        Allocator _allocator;
        const TileGridMap& _tileGridMap;
        ObjectPool<Entity> _entityPool;
        unique_ptr<World> _world;
    };
    
} /* namespace ovengine */ } /* namespace cinekine */

#endif /* defined(__Overview__Simulation__) */
