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

#include "Engine/Model/Stage.hpp"
#include "Engine/Model/SpriteDatabaseLoader.hpp"
#include "Engine/Model/TileDatabaseLoader.hpp"
#include "Engine/Model/StageSpriteInstance.hpp"

#include "Graphics/BitmapLibrary.hpp"
#include "Graphics/FontLibrary.hpp"

#include "Core/FileStreamBuf.hpp"


namespace cinekine {
    namespace prototype {

    GameScene::GameScene(SceneController& sceneController) :
        _allocator(),
        _sceneController(sceneController),
        _ui(_sceneController.ui()),
        _renderer(_sceneController.renderer()),
        _bitmapLibrary(_renderer, _allocator),
        _fontLibrary(_renderer, 1, _allocator),
        _tileDb(1024, _allocator),
        _spriteDb(32, _allocator),
        _architect(),
        _gameView(),
        _window()
    {
        //  debug font
      
        _fontLibrary.loadFont(glx::kFontHandle_Default, "static/fonts/DroidSans.ttf", 16);
        _gameView = std::allocate_shared<GameView,
                                         std_allocator<GameView>,
                                         glx::RendererCLI&,
                                         glx::FontLibrary&,
                                         glx::BitmapLibrary&>
                            (
                                std_allocator<GameView>(_allocator),
                                _renderer, _fontLibrary, _bitmapLibrary
                            );

        _window = _ui.createWindow("static/ui/main.rml",
                [this](const char* className, const char* idName)
                {
                    return _gameView;
                }
            );
        _window->setEventListener(this);
        _window->show();

        ovengine::MapBounds bounds = { 16, 16, 1 };
        ovengine::Stage::ResourceCounts resourceCounts;
        resourceCounts.spriteLimit = 256;
        _stage = std::allocate_shared<ovengine::Stage,
                                    std_allocator<ovengine::Stage>,
                                    const ovengine::TileDatabase&,
                                    const ovengine::SpriteDatabase&,
                                    const ovengine::Stage::ResourceCounts&,
                                    const ovengine::MapBounds&,
                                    const Allocator&>
                            (
                                std_allocator<ovengine::Stage>(_allocator),
                                _tileDb,
                                _spriteDb,
                                resourceCounts,
                                bounds,
                                _allocator
                            );
        
        
        loadTileDatabase("tiles_caves.json");
        loadSpriteDatabase("sprites_common.json");
        _architect = unique_ptr<Architect>(
                        _allocator.newItem<Architect>(*_stage,
                                                      _tileDb,
                                                      _allocator),
                        _allocator);

        //  prepopulate map.
        auto* tilemap = _stage->tilemapAtZ(0);
        for (uint32_t row = 0; row < tilemap->rowCount(); ++row)
        {
            ovengine::Tilemap::row_strip tileRow = tilemap->atRow(row, 0);
            while (tileRow.first != tileRow.second)
            {
                tileRow.first->layer[0] = 0x0002;
                ++tileRow.first;
            }
        }
        
        
        _viewPos = glm::vec3(bounds.xUnits * 0.5f, bounds.yUnits * 0.5f, 0.f);
        
        auto avatarSprite = _stage->createSpriteInstance("warrior", _viewPos);
        
        _gameView->setStage(_stage, _viewPos);
    }
        
    void GameScene::loadTileDatabase(const char* filename)
    {
        FileStreamBuf dbStream(filename);
        if (!dbStream)
            return;
        
        ovengine::TileDatabaseLoader tileDbLoader(_tileDb);
        tileDbLoader.unserialize(dbStream,
            [this](const char* atlasName) -> cinek_bitmap_atlas
            {
                char path[MAX_PATH];
                snprintf(path, sizeof(path), "textures/%s", atlasName);
                return _bitmapLibrary.loadAtlas(path);
            },
            [this](cinek_bitmap_atlas atlas, const char* name) -> cinek_bitmap_index
            {
                const glx::BitmapAtlas* bitmapAtlas = _bitmapLibrary.getAtlas(atlas).get();
                if (!bitmapAtlas)
                    return kCinekBitmapIndex_Invalid;
                return bitmapAtlas->getBitmapIndex(name);
            });
    }
    
    void GameScene::loadSpriteDatabase(const char* filename)
    {
        FileStreamBuf dbStream(filename);
        if (!dbStream)
            return;
        
        ovengine::SpriteDatabaseLoader spriteDbLoader(_spriteDb);
        spriteDbLoader.unserialize(dbStream,
            [this](const char* atlasName) -> cinek_bitmap_atlas
            {
               char path[MAX_PATH];
               snprintf(path, sizeof(path), "textures/%s", atlasName);
               return _bitmapLibrary.loadAtlas(path);
            },
            [this]( cinek_bitmap_atlas atlas, const char* name) -> cinek_bitmap_index
            {
               const glx::BitmapAtlas* bitmapAtlas = _bitmapLibrary.getAtlas(atlas).get();
               if (!bitmapAtlas)
                   return kCinekBitmapIndex_Invalid;
               return bitmapAtlas->getBitmapIndex(name);
            });
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
