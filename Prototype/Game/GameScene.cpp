//
//  GameScene.cpp
//  Overview
//
//  Created by Samir Sinha on 12/4/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "GameScene.hpp"
#include "GameView.hpp"

#include "Prototype/SceneController.hpp"

#include "Engine/Model/Stage.hpp"
#include "Engine/Model/SpriteLibraryLoader.hpp"
#include "Engine/Model/TileCollectionLoader.hpp"
#include "Engine/Model/StageSpriteInstance.hpp"

#include "Graphics/BitmapLibrary.hpp"
#include "Graphics/FontLibrary.hpp"

#include "Core/FileStreamBuf.hpp"
#include "Core/StreamBufRapidJson.hpp"


namespace cinekine {
    namespace prototype {

    GameScene::GameScene(SceneController& sceneController) :
        _allocator(),
        _sceneController(sceneController),
        _ui(_sceneController.ui()),
        _renderer(_sceneController.renderer()),
        _bitmapLibrary(_renderer, _allocator),
        _fontLibrary(_renderer, 1, _allocator),
        _tileLibrary(1024, _allocator),
        _spriteLibrary(32, _allocator),
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
        
        //  load game document
        FileStreamBuf gameStream("game.json");
        RapidJsonStdStreamBuf jsonStream(gameStream);
        _gameDocument.ParseStream<0>(jsonStream);

        //  create our UI
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
                                    const ovengine::TileLibrary&,
                                    const ovengine::SpriteLibrary&,
                                    const ovengine::Stage::ResourceCounts&,
                                    const ovengine::MapBounds&,
                                    const Allocator&>
                            (
                                std_allocator<ovengine::Stage>(_allocator),
                                _tileLibrary,
                                _spriteLibrary,
                                resourceCounts,
                                bounds,
                                _allocator
                            );
        
        
        loadTileCollection("tiles_dungeon.json");
        loadSpriteCollection("sprites_common.json");
        
        _viewPos = glm::vec3(bounds.xUnits * 0.5f, bounds.yUnits * 0.5f, 0.f);
        
        auto avatarSprite = _stage->createSpriteInstance("warrior", _viewPos);
        
        _gameView->setStage(_stage, _viewPos);
    }
        
    void GameScene::loadTileCollection(const char* filename)
    {
        FileStreamBuf dbStream(filename);
        if (!dbStream)
            return;
        
        ovengine::TileCollectionLoader tileLoader(_gameDocument["model"]["tiles"]["flags"],
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
            },
            [this](ovengine::TileCollection&& collection)
            {
                _tileLibrary.mapCollectionToSlot(std::move(collection), 0);
            },
            _allocator);
        unserializeFromJSON(dbStream, tileLoader);
    }
    
    void GameScene::loadSpriteCollection(const char* filename)
    {
        FileStreamBuf dbStream(filename);
        if (!dbStream)
            return;
        
        unserializeFromJSON(_spriteLibrary, dbStream,
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
    }

    }   // namespace ovengine
}   // namespace cinekine
