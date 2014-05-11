//
//  GameScene.cpp
//  Overview
//
//  Created by Samir Sinha on 12/4/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "GameScene.hpp"
#include "GameView.hpp"
#include "Architect.hpp"

#include "Prototype/SceneController.hpp"

#include "Engine/TheaterCLI.hpp"
#include "Engine/Map.hpp"

#include "Graphics/BitmapLibrary.hpp"
#include "Graphics/FontLibrary.hpp"


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
        _architect(),
        _map(),
        _gameView(),
        _window()
    {
        //  debug font
      
        _fontLibrary.loadFont(glx::kFontHandle_Default, "static/fonts/DroidSans.ttf", 16);
        _theater.loadTileDatabase("tiles_caves.json", _bitmapLibrary);
        _theater.loadSpriteDatabase("sprites_common.json", _bitmapLibrary);
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
        _window->setEventListener(this);
        _window->show();

        cinek_ov_map_bounds bounds = { 16, 16, 1 };

        _map = std::allocate_shared<ovengine::Map,
                                    std_allocator<ovengine::Map>,
                                    const cinek_ov_map_bounds&,
                                    const Allocator&>
                            (
                                std_allocator<ovengine::Map>(_allocator),
                                bounds,
                                _allocator
                            );
        _architect = unique_ptr<Architect>(
                        _allocator.newItem<Architect>(*_map,
                                                      _theater.tileDatabase(),
                                                      _allocator),
                        _allocator);

        _viewPos = glm::vec3(bounds.xUnits * 0.5f, bounds.yUnits * 0.5f, 0.f);

        //  prepopulate map.
        auto* tilemap = _map->tilemapAtZ(0);
        for (uint32_t row = 0; row < tilemap->rowCount(); ++row)
        {
            ovengine::Tilemap::row_strip tileRow = tilemap->atRow(row, 0);
            while (tileRow.first != tileRow.second)
            {
                tileRow.first->layer[0] = 0x0002;
                ++tileRow.first;
            }
        }

        _gameView->setMap(_map, _viewPos);
    }

    void GameScene::onKeyDown(SDL_Keycode keycode, uint16_t keymod)
    {
        glm::vec3 newPos(0,0,0);
        
        const float kAdjX = 0.25f;
        const float kAdjY = 0.25f;
        
        switch (keycode)
        {
        case SDLK_e:    newPos.y = -kAdjY; break;
        case SDLK_z:    newPos.y = kAdjY; break;
        case SDLK_q:    newPos.x = -kAdjX; break;
        case SDLK_c:    newPos.x = kAdjX; break;
        case SDLK_w:    newPos.x = -kAdjX; newPos.y = -kAdjY; break;
        case SDLK_x:    newPos.x = kAdjX; newPos.y = kAdjY; break;
        case SDLK_a:    newPos.x = -kAdjX; newPos.y = kAdjY; break;
        case SDLK_d:    newPos.x = kAdjX; newPos.y = -kAdjY; break;
        default:
            break;
        }

        if (newPos.x || newPos.y || newPos.z)
        {
            _viewPos += newPos;
            _gameView->setMapPosition(_viewPos);
        }
    }

    void GameScene::onKeyUp(SDL_Keycode keycode, uint16_t keymod)
    {

    }

    void GameScene::update()
    {
        _architect->update();
    }

    }   // namespace ovengine
}   // namespace cinekine
