//
//  StaticWorldMap.h
//  Overview
//
//  Created by Samir Sinha on 12/23/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Model_Static_World_Map_hpp
#define Overview_Model_Static_World_Map_hpp

#include "Shared/GameTemplates.hpp"
#include "Engine/Model/AABB.hpp"
#include "Engine/Model/TileGridMap.hpp"
#include "cinek/allocator.hpp"

namespace cinek {

namespace overview {

    class GameTemplates;
    class RoomGraph;

    class StaticWorldMap
    {
        CK_CLASS_NON_COPYABLE(StaticWorldMap);
        
    public:
        StaticWorldMap(const GameTemplates& templates);
        ~StaticWorldMap();
        
        void loadTileGridMap(unique_ptr<TileGridMap>&& tileGridMap);
        void loadRoomGraph(unique_ptr<RoomGraph>&& roomGraph);
    
    
        AABB<Point> bounds() const;
    
        const TileLibrary& tileLibrary() const {
            return _templates->tileLibrary();
        }
    
        const TileGridMap& tileGridMap() const {
            return *_tileGridMap;
        }
        const RoomGraph& roomGraph() const {
            return *_roomGraph;
        }
        
    private:
        const GameTemplates* _templates;
        unique_ptr<TileGridMap> _tileGridMap;
        unique_ptr<RoomGraph> _roomGraph;
    };

} /* namespace overview */ } /* namespace cinek */

#endif
