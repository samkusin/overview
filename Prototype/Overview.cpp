//
//  Overview.cpp
//  Overview
//
//  Created by Samir Sinha on 8/22/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "Overview.hpp"
#include "Engine/TheaterClient.hpp"

#include "cinek/cpp/ckalloc.hpp"

#include "cinek/overview/stage.hpp"
#include "cinek/overview/viewpoint.hpp"

#include "SceneController.hpp"
#include "Scenes/GameScene.hpp"
#include "Engine/UIClient.hpp"

#include <memory>


namespace cinekine {
    namespace ovengine {
    
    ovengine::Director* CreateDirector(TheaterClient& cli, WindowComponentClient& windowCLI)
    {
        Allocator allocator;
        return allocator.newItem<prototype::Overview>(cli, windowCLI);
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

    Overview::Overview(ovengine::TheaterClient& cli, ovengine::WindowComponentClient& uiCLI) :
        _theaterCLI(cli),
        _UICLI(uiCLI),
        _allocator(),
        _sceneController(_allocator),
        _stage({ 17, 17, 0, 0 }, _allocator)
    {
        _theaterCLI.loadTileDatabase("dungeontiles");
        _theaterCLI.loadSpriteDatabase("sprites");
        
        _sceneController.add( "game",
            [this]() -> std::shared_ptr<Scene>
            {
                unique_ptr<ovengine::UIWindow> window(_UICLI.createWindow("static/ui/main.rml"));
                return std::allocate_shared<GameScene, std_allocator<GameScene>, unique_ptr<ovengine::UIWindow>>
                        (
                            std_allocator<GameScene>(_allocator),
                            std::move(window)
                        );
            });
        
        _sceneController.next("game");
        
        std::shared_ptr<overview::Map> map = _stage.getMapPtr();
        const cinek_ov_map_bounds& bounds = map->getMapBounds();
        _viewPos = glm::vec3(bounds.xUnits * 0.5f, bounds.yUnits * 0.5f, 0.f);
        
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
        _sceneController.update();
        
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
