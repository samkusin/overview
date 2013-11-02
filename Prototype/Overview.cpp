//
//  Overview.cpp
//  Overview
//
//  Created by Samir Sinha on 8/22/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "Overview.hpp"
#include "Engine/TheaterClient.hpp"
#include "UI/IMGUIClient.hpp"

#include "cinek/cpp/ckalloc.hpp"

#include "cinek/overview/stage.hpp"
#include "cinek/overview/viewpoint.hpp"

namespace cinekine {
    namespace ovengine {
    
    ovengine::Director* CreateDirector(TheaterClient& cli,
                                       imgui::ClientInterface& imgui)
    {
        Allocator allocator;
        return allocator.newItem<prototype::Overview>(cli, imgui);
    }
    
    void DestroyDirector(ovengine::Director* director)
    {
        Allocator allocator;
        allocator.deleteItem(director);
    }
    
    }   // namespace ovengine
}

namespace cinekine {
    namespace prototype {

    Overview::Overview(ovengine::TheaterClient& cli,
                       imgui::ClientInterface& imgui) :
        _theaterCLI(cli),
        _imguiCLI(imgui),
        _allocator(),
        _stage({ 17, 17, 0, 0 }, _allocator),
        _scrollPos1(0)
    {
        _theaterCLI.loadTileDatabase("dungeontiles");
        _theaterCLI.loadSpriteDatabase("sprites");
        
        std::shared_ptr<overview::Map> map = _stage.getMapPtr();
        const cinek_ov_map_bounds& bounds = map->getMapBounds();
        _viewPos = { bounds.xUnits * 0.5f, bounds.yUnits * 0.5f, 0.f };
        
        //  prepopulate map.
        overview::Tilemap* tilemap = map->getTilemapAtZ(0);
        for (uint32_t row = 0; row < tilemap->getRowCount(); ++row)
        {
            overview::Tilemap::row_strip tileRow = tilemap->atRow(row, 0);
            while (tileRow.first != tileRow.second)
            {
                *(tileRow.first) = 0x0000;
                ++tileRow.first;
            }
        }
                
        _theaterCLI.setViewMap(map, _viewPos);
    }
    
    Overview::~Overview()
    {
        _theaterCLI.clearViewMap();
    }
    
    void Overview::update()
    {
        _imguiCLI.beginScrollArea({ 20, 20, 384, 384 }, _scrollPos1);
        _imguiCLI.separator();
        _imguiCLI.separatorLine();
        _imguiCLI.label("This is a label.");
        _scrollPos1 = _imguiCLI.endScrollArea();

        /*std::shared_ptr<overview::Map> map = _stage.getMapPtr();
        //  prepopulate map.
        overview::Tilemap* tilemap = map->getTilemapAtZ(0);
        for (uint32_t row = 0; row < tilemap->getRowCount(); ++row)
        {
            overview::Tilemap::row_strip tileRow = tilemap->atRow(row, 0);
            while (tileRow.first != tileRow.second)
            {
                *(tileRow.first) = _cycle % 3;
                ++tileRow.first;
            }
        }
        */
 
    }

    
    }   //  namespace prototype
}   // namespace cinekine
