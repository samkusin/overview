//
//  GameView.cpp
//  Overview
//
//  Created by Samir Sinha on 8/26/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "View/GameView.hpp"
#include "Game/Simulation.hpp"
#include "Game/EntitySimResult.hpp"

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

namespace cinek {
    namespace overview {

    const int32_t TILE_WIDTH = 32;
    const int32_t TILE_HEIGHT = 16;

    GameView::GameView(ApplicationController& application,
                       const Allocator& allocator) :
        _application(application),
        _allocator(allocator),
        _bitmapLibrary(_application.renderer(), _allocator),
        _fontLibrary(_application.renderer(), 1, _allocator),
        _graphics(_application.renderer(), _bitmapLibrary, _fontLibrary),
        _sprites(1024, _allocator),
        _entitySpritePtrs(_allocator)
    {
        //  this should be an application-wide op (move _fontLibrary to ApplicationController)
        _fontLibrary.loadFont(glx::kFontHandle_Default, "static/fonts/DroidSans.ttf", 16);

        //  load game document
        overview::GameTemplates::InitParams initParams;
        initParams.gameDefinitionPath = "game.json";
        initParams.tileSlotLimit = 32;
        initParams.spriteLimit = 256;

        _gameTemplates = allocate_unique<overview::GameTemplates>(_allocator, initParams, _allocator);

        _gameTemplates->loadTileCollection("tiles_dungeon.json", _bitmapLibrary);
        _gameTemplates->loadSpriteCollection("sprites_core.json", _bitmapLibrary);
        _gameTemplates->loadEntityTemplateCollection("entity.json");


        GenerateMapParams generateMapParams;
        generateMapParams.blocksPathname = "blocks.json";
        generateMapParams.floorX = 12;
        generateMapParams.floorY = 12;
        generateMapParams.overlayToFloorRatio = 4;
        generateMapParams.roomLimit = 8;

        generateMapFromTemplates(*_gameTemplates, generateMapParams);

        _stage = allocate_unique<overview::Stage>(_allocator, *_gameTemplates);
        _entitySpritePtrs.reserve(1024);

        //  create our scene graph from the stage
        auto viewport = _application.renderer().getViewport();
        glm::ivec2 viewDimensions(viewport.width(), viewport.height());
        glm::ivec2 tileDimensions(TILE_WIDTH, TILE_HEIGHT);

        auto isoScenePtr = _allocator.newItem<overview::IsoScene>(viewDimensions,
                                                                  tileDimensions,
                                                                  *_stage,
                                                                  _allocator);
        _isoScene = unique_ptr<overview::IsoScene>(isoScenePtr, _allocator);


        auto overlayDims = _stage->tileGridMap().overlayDimensions();
        _viewPos = Point(overlayDims.x * 0.5f, overlayDims.y * 0.5f, 0.f);
        
        //  create simulation using GameTemplates
        CreateSimulationParams simParams;
        _simulation = std::move(generateSimulation(*_gameTemplates, simParams));
        
        EntityId id = _simulation->createEntity("avatar");
        _simulation->activateEntity(id, _viewPos, Point(0,1,0));
    }

    GameView::~GameView()
    {
        for (auto& sprite: _entitySpritePtrs)
        {
            _stage->detachSpriteInstance(sprite.second);
            _sprites.destruct(sprite.second);
        }
        _entitySpritePtrs.clear();
    }

    void GameView::update(uint32_t ticks)
    {
        // update systems (at some point these could be parallel jobs, so
        // it doesn't matter what order thse updates are called)
        _isoScene->update(ticks, _viewPos);
        _simulation->update(ticks);
        
        // execute upon system update complete (this would be the sync
        // point between all parallel jobs in an update frame)
        /*
        _simulation->syncResults(std::bind(&GameView::handleSimResults,
                                     this,
                                     std::placeholders::_1));
        */
        _simulation->syncResults([this](const EventBase* evt) {
            handleSimResults(evt);
        });
    }
    
    void GameView::handleSimResults(const EventBase* evt)
    {
        if (evt->classID() == EntitySimResult::kClassID) 
        {
            auto result = event_cast<const EntitySimResult>(evt);
            if (result)
            {
                entitySimResult(result);
            }
        }
    }
    
    void GameView::entitySimResult(const EntitySimResult* result)
    {
        if (result->flags() & kEntityResultFlag_Activated)
        {
            auto entity = _simulation->entity(result->id());
            if (entity)
            {
                auto sprite = allocateSprite(entity->sourceTemplate().spriteName());
                _entitySpritePtrs.emplace(result->id(), sprite);
                auto body = entity->body();
                applyObjectStateToSprite(sprite, *body, result->timestamp());
                _stage->attachSpriteInstance(sprite);
            }
        }
        if (result->flags() & kEntityResultFlag_Deactivated)
        {
            auto spriteIt = _entitySpritePtrs.find(result->id());
            if (spriteIt != _entitySpritePtrs.end())
            {
                _stage->detachSpriteInstance(spriteIt->second);
                freeSprite(spriteIt->second);
                _entitySpritePtrs.erase(spriteIt);
            }
        }
    }
    
    SpriteInstancePtr GameView::allocateSprite(const std::string& spriteClassName)
    {
        auto& spriteClass = _gameTemplates->spriteLibrary().spriteByName(spriteClassName);
        SpriteInstancePtr ptr = _sprites.construct(spriteClass);
        return ptr;
    }
    
    void GameView::freeSprite(SpriteInstancePtr ptr)
    {
        _sprites.destruct(ptr);
    }
    
    void GameView::applyObjectStateToSprite(SpriteInstancePtr sprite,
                                            const WorldObject& body,
                                            uint32_t timeMs)
    {
        //  direction using (0,1,0) as a reference to South
        //  dir.x determines the side ('west', 'east')
        //  dot product determines direction.
        //
        auto& dir = body.frontDirection();
        float speed = body.speed();
        float dp = glm::dot(dir, Point(0,1,0));
        
        AnimationStateId animId = kAnimationState_Idle;
        if (dp >= 0.75f)
        {
            animId = speed > 0.f ? kAnimationState_Walk_South : kAnimationState_Idle_South;
        }
        else if (dp >= 0.25f)
        {
            if (dir.x >= 0.f)
                animId = speed > 0.f ? kAnimationState_Walk_SouthEast : kAnimationState_Idle_SouthEast;
            else
                animId = speed > 0.f ? kAnimationState_Walk_SouthWest : kAnimationState_Idle_SouthWest;
        }
        else if (dp >= -0.25f)
        {
            if (dir.x >= 0.f)
                animId = speed > 0.f ? kAnimationState_Walk_East : kAnimationState_Idle_East;
            else
                animId = speed > 0.f ? kAnimationState_Walk_West : kAnimationState_Idle_West;
        }
        else if (dp >= -0.75f)
        {
            if (dir.x >= 0.f)
                animId = speed > 0.f ? kAnimationState_Walk_NorthEast : kAnimationState_Idle_NorthEast;
            else
                animId = speed > 0.f ? kAnimationState_Walk_NorthWest : kAnimationState_Idle_NorthWest;
        }
        else
        {
            animId = speed > 0.f ? kAnimationState_Walk_North : kAnimationState_Idle_North;
        }
        

        sprite->setState(animId, timeMs);
        sprite->setPosition(body.position());
    }

    //  Main render pipeline
    //
    void GameView::render()
    {
        renderReset();

        _isoScene->visit([this](const overview::IsoNode* node)
         {
              auto screenPos = node->viewPos();

              switch(node->type())
              {
              case overview::IsoNode::kBitmap:
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
            //_playerSprite->setPosition(_viewPos);
        }
    }

    void GameView::onKeyUp(SDL_Keycode keycode, uint16_t keymod)
    {

    }

    }
}