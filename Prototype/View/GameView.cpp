//
//  GameView.cpp
//  Overview
//
//  Created by Samir Sinha on 8/26/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "View/GameView.hpp"
#include "Game/Simulation.hpp"

#include "Builder/GameMapGenerator.hpp"
#include "Builder/SimulationGenerator.hpp"

#include "ApplicationController.hpp"

#include "View/Render/IsoScene.hpp"
#include "Game/EntityTypes.hpp"

#include "Engine/Debug.hpp"
#include "Graphics/RendererCLI.hpp"

#include "Engine/Model/SpriteLibraryLoader.hpp"
#include "Engine/Model/TileCollectionLoader.hpp"
#include "Core/FileStreamBuf.hpp"
#include "Core/StreamBufRapidJson.hpp"

#include "cinek/allocator.hpp"

#include <random>

#include "Core/ValueConvertGLM.hpp"

/////////////////////////////////////////////////////////////

namespace cinekine {
    namespace prototype {

    const int32_t TILE_WIDTH = 32;
    const int32_t TILE_HEIGHT = 16;

    GameView::GameView(ApplicationController& application,
                       const Allocator& allocator) :
        _application(application),
        _allocator(allocator),
        _bitmapLibrary(_application.renderer(), _allocator),
        _fontLibrary(_application.renderer(), 1, _allocator),
        _graphics(_application.renderer(), _bitmapLibrary, _fontLibrary),
        _spritePool(256, _allocator)
    {
        //  this should be an application-wide op (move _fontLibrary to ApplicationController)
        _fontLibrary.loadFont(glx::kFontHandle_Default, "static/fonts/DroidSans.ttf", 16);

        //  load game document
        ovengine::GameTemplates::InitParams initParams;
        initParams.gameDefinitionPath = "game.json";
        initParams.tileSlotLimit = 32;
        initParams.spriteLimit = 256;
        
        _gameTemplates = allocate_unique<ovengine::GameTemplates>(_allocator, initParams, _allocator);
        
        _gameTemplates->loadTileCollection("tiles_dungeon.json", _bitmapLibrary);
        _gameTemplates->loadSpriteCollection("sprites_core.json", _bitmapLibrary);
        
        
        GenerateMapParams generateMapParams;
        generateMapParams.blocksPathname = "blocks.json";
        generateMapParams.floorX = 12;
        generateMapParams.floorY = 12;
        generateMapParams.overlayToFloorRatio = 4;
        generateMapParams.roomLimit = 8;
        
        generateMapFromTemplates(*_gameTemplates, generateMapParams);
        
        _stage = allocate_unique<ovengine::Stage>(_allocator, *_gameTemplates);

       
        //  create our scene graph from the stage
        auto viewport = _application.renderer().getViewport();
        glm::ivec2 viewDimensions(viewport.width(), viewport.height());
        glm::ivec2 tileDimensions(TILE_WIDTH, TILE_HEIGHT);
        
        auto isoScenePtr = _allocator.newItem<ovengine::IsoScene>(viewDimensions,
                                                                  tileDimensions,
                                                                  *_stage,
                                                                  _allocator);
        _isoScene = unique_ptr<ovengine::IsoScene>(isoScenePtr, _allocator);

        
        //  create our player entity
        auto overlayDims = _stage->tileGridMap().overlayDimensions();
        _viewPos = Point(overlayDims.x * 0.5f, overlayDims.y * 0.5f, 0.f);
        
        auto& maleAvatarSprite = _gameTemplates->spriteLibrary().spriteByName("warrior");
        _playerSprite = _spritePool.construct(maleAvatarSprite);
        _playerSprite->setState(kAnimationState_Walk_South, 0);
        _playerSprite->setPosition(_viewPos);
        _stage->attachSpriteInstance(_playerSprite);

        
        /*
        std::default_random_engine numGenerator;
        std::uniform_int_distribution<int> xDist(0, overlayDims.x-1);
        std::uniform_int_distribution<int> yDist(0, overlayDims.y-1);
        std::uniform_int_distribution<int> stateDist(kAnimationState_Idle_North,
                                                     kAnimationState_Walk_NorthWest);
        _otherSprites.reserve(256);
        
        for (int cnt = 0; cnt < _otherSprites.capacity(); ++cnt)
        {
            auto sprite = _spritePool.construct(maleAvatarSprite);
            sprite->setState(stateDist(numGenerator), 0);
            sprite->setPosition(Point(xDist(numGenerator), yDist(numGenerator), 0.f));
            _stage->attachSpriteInstance(sprite);
            _otherSprites.push_back(sprite);
        }
        
        ovengine::EntityCommand cmd(2);
        cmd.set<int32_t>(10, 12);
        cmd.set<float>(3, 3.14159f);
        cmd.set<glm::vec3>(9, glm::vec3(1.0f, 0.5f, 0.25f));
        
        printf("[10] = %d\n", cmd.get<int32_t>(10));
        
        auto vec = cmd.get<glm::vec3>(9);
        printf("[9] = %.2f,%.2f,%.2f\n", vec.x, vec.y, vec.z);
        printf("[3] = %s\n", cmd.get<std::string>(3).c_str());
        */
        //  create simulation using GameTemplates
        CreateSimulationParams simParams;
        simParams.allocator = _allocator;
        simParams.entityLimit = 16;
        _simulation = std::move(generateSimulation(*_gameTemplates, simParams));
    }

    GameView::~GameView()
    {
        while (!_otherSprites.empty())
        {
            auto sprite = _otherSprites.back();
            _stage->detachSpriteInstance(sprite);
            _otherSprites.pop_back();
        }

        _stage->detachSpriteInstance(_playerSprite);
        _playerSprite = nullptr;
    }
    
    void GameView::update(uint32_t ticks)
    {
        _isoScene->update(ticks, _viewPos);
    }
        
   

    //  Main render pipeline
    //
    void GameView::render()
    {
        renderReset();
        
        _isoScene->visit([this](const ovengine::IsoNode* node)
         {
              auto screenPos = node->viewPos();
              
              switch(node->type())
              {
              case ovengine::IsoNode::kBitmap:
                  {
                      auto bitmap = node->bitmap();
                      auto atlas = _bitmapLibrary.getAtlas(bitmap.bmpAtlas);
                      if (atlas)
                      {
                          auto bitmapInfo = atlas->getBitmapFromIndex(bitmap.bmpIndex);
                          if (bitmapInfo)
                          {
                              renderBitmap(atlas->getTexture(), *bitmapInfo,
                                           screenPos.x,
                                           screenPos.y);
                          }
                      }
                  }
                  break;
              default:
                  break;
              }
         });

        
        glx::Style style;
        style.textColor = glx::RGBAColor(255,0,255,255);
        style.textFont = glx::kFontHandle_Default;
        style.fillMethod = glx::kFillMethod_SolidFill;
        style.fillColor = glx::RGBAColor(0,0,255,255);
        _graphics.drawText("Welcome to the overview 2D project by Samir Sinha.",
                            20, 160, style);
        /*
        _graphics.drawRect(glx::Rect(100,100,200,200), style);

        glm::ivec2 polyVerts[5];
        polyVerts[0].x = 300;
        polyVerts[0].y = 100;
        polyVerts[1].x = 375;
        polyVerts[1].y = 175;
        polyVerts[2].x = 340;
        polyVerts[2].y = 250;
        polyVerts[3].x = 260;
        polyVerts[3].y = 250;
        polyVerts[4].x = 225;
        polyVerts[4].y = 175;
        style.fillColor = glx::RGBAColor(255,0,0,255);
        _graphics.drawPolygon(polyVerts, 5, style);
        */
    }
        
    void GameView::renderReset()
    {
    }
        
    void GameView::renderBitmap(const glx::Texture& texture, const glx::BitmapInfo& bitmap,
                                int32_t sx, int32_t sy)
    {
        glx::RGBAColor color(255,255,255,(uint8_t)(1.0*255));
        glx::Rect srcRect(bitmap.x,
                          bitmap.y,
                          bitmap.x + bitmap.w,
                          bitmap.y + bitmap.h);
        glx::Rect destRect = glx::Rect::rectFromDimensions(sx + bitmap.offX,
                                                           sy + bitmap.offY - bitmap.srcH,
                                                           bitmap.offW,
                                                           bitmap.offH);
        _application.renderer().drawTextureRect(texture, srcRect, destRect, color);
    }
        
    void GameView::onMouseButtonDown(MouseButton button, int32_t x, int32_t y)
    {

    }

    void GameView::onMouseButtonUp(MouseButton button, int32_t x, int32_t y)
    {

    }

    void GameView::onMouseMove(MouseRegion region, int32_t x, int32_t y)
    {
    }
        
    void GameView::onKeyDown(SDL_Keycode keycode, uint16_t keymod)
    {
        glm::vec3 newPos(0,0,0);
        
        const float kAdjX = 0.125f;
        const float kAdjY = 0.125f;
        
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
            _playerSprite->setPosition(_viewPos);
        }
    }
    
    void GameView::onKeyUp(SDL_Keycode keycode, uint16_t keymod)
    {
        
    }

    }
}