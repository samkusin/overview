//
//  World.hpp
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_Model_World_hpp
#define Overview_Game_Model_World_hpp

#include "Game/SimulationTypes.hpp"

#include "Engine/Model/AABB.hpp"

#include "cinek/allocator.hpp"
#include "cinek/vector.hpp"

namespace cinek {

    class MessageQueue;
    
    namespace overview {
        class Entity;
        class WorldObject;
        class StaticWorldMap;
    }
}

namespace cinek {
    namespace overview {
    
    class World
    {
        CK_CLASS_NON_COPYABLE(World);

    public:
        struct CreateParams
        {
            const StaticWorldMap* staticWorldMap = nullptr;
            uint32_t objectLimit = 128;
            bool visualDebug = false;
        };
        World(const CreateParams& params, Allocator& allocator);
        ~World();
        
        WorldObject* createObject(const Point& pos,
                                  const Point& front,
                                  const AABB<Point>& bbox);
        void destroyObject(WorldObject* body);

        void update(MessageQueue& eventQueue, uint32_t deltaTimeMs);
        
        void appendDebugMessages(vector<SimDebugMessage>& output);

    private:
        class Impl;
        unique_ptr<Impl> _impl;
    };

    }
}

#endif /* ifdef Overview_Game_Model_World_hpp */
