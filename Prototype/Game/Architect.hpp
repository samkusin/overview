//
//  Architect.hpp
//  Overview
//
//  Created by Samir Sinha on 5/8/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Architect_hpp
#define Overview_Architect_hpp

#include "Engine/Builder/Builder.hpp"
#include "Engine/Model/TileDatabase.hpp"

#include "cinek/allocator.hpp"

namespace cinekine { namespace prototype {
    
    class Architect
    {
    public:
        Architect(ovengine::Map& map,
                  const ovengine::TileDatabase& tileTemplates,
                  const Allocator& allocator);

        void update();
        
        void newRegion(const ovengine::Box& mapBounds);
        
        void newConnection(uint32_t roomA, uint32_t roomB);
        
        enum Status
        {
            kState_BuildRegions,
            kState_BuildConnections,
            kState_Done
        };
        Status status() const { return _state; }

        
    private:
        Allocator _allocator;

        Status _state;
        
        struct Room
        {
            int handle;
            ovengine::Box box;
        };
        
        const ovengine::TileDatabase& _tileTemplates;
        ovengine::Map& _map;
        unique_ptr<ovengine::Builder> _builder;
        vector<Room> _rooms;
        
        //  determines whether we can build a room from the specified box region
        bool canBuildRoom(const ovengine::Box& bounds,
                          const ovengine::Box& box) const;
        
        
        //  randomize a box.  one or more boxes comprise a room
        auto generateBox(int32_t wMin, int32_t hMin,
                         int32_t wMax, int32_t hMax) -> ovengine::Box;
        
    };


    
} /* namespace prototype */ } /* namespace cinekine */

#endif

