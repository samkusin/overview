//
//  World.hpp
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_Model_World_hpp
#define Overview_Game_Model_World_hpp

#include "Game/Model/WorldTypes.hpp"
#include "Engine/Model/AABB.hpp"
#include "Engine/Model/ModelTypes.hpp"

#include "cinek/allocator.hpp"

namespace cinekine {
    namespace ovengine {

        
    class World
    {
    public:
        World(const AABB<Point>& bounds, const Allocator& allocator);
        ~World();
        
    private:
        Allocator _allocator;
        class Impl;
        unique_ptr<Impl> _impl;
    };
        
    }
}

#endif /* ifdef Overview_Game_Model_World_hpp */
