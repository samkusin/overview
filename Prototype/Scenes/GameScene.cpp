//
//  GameScene.cpp
//  Overview
//
//  Created by Samir Sinha on 12/4/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "./GameScene.hpp"

#include "../SceneController.hpp"
#include "../Views/GameView.hpp"

#include "Engine/TheaterCLI.hpp"

#include "Graphics/BitmapLibrary.hpp"
#include "Graphics/FontLibrary.hpp"

#include "cinek/overview/map.hpp"

namespace cinekine {
    namespace prototype {
        
    GameScene::GameScene(SceneController& sceneController) :
        _allocator(),
        _sceneController(sceneController),
        _theater(_sceneController.theater()),
        _ui(_sceneController.ui()),
        _renderer(_sceneController.renderer()),
        _bitmapLibrary(_renderer, _allocator),
        _fontLibrary(_renderer, 1, _allocator),
        _stage({ 17, 17, 0, 0 }, _allocator),
        _gameView(),
        _window()
    {
        _theater.loadTileDatabase("dungeontiles", _bitmapLibrary);
        _theater.loadSpriteDatabase("sprites", _bitmapLibrary);
        _gameView = std::allocate_shared<GameView,
                                         std_allocator<GameView>,
                                         ovengine::TheaterCLI&,
                                         glx::RendererCLI&,
                                         glx::FontLibrary&,
                                         glx::BitmapLibrary&>
                            (
                                std_allocator<GameView>(_allocator),
                                _theater, _renderer, _fontLibrary, _bitmapLibrary
                            );
       
        _window = _ui.createWindow("static/ui/main.rml",
                [this](const char* className, const char* idName)
                {
                    return _gameView;
                }
            );
        _window->show();

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

        _gameView->setMap(map, _viewPos);
    }
    
    void GameScene::update()
    {    
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
        
    }   // namespace ovengine
}   // namespace cinekine
