//
//  GameView.cpp
//  Overview
//
//  Created by Samir Sinha on 8/26/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "View/GameView.hpp"
#include "Shared/StaticWorldMap.hpp"
#include "Game/Simulation.hpp"

#include "Builder/GameMapGenerator.hpp"
#include "Builder/SimulationGenerator.hpp"

#include "ApplicationController.hpp"

#include "View/Render/IsoScene.hpp"

#include "Engine/Debug.hpp"
#include "Graphics/RendererCLI.hpp"

#include "Engine/Model/SpriteCollectionLoader.hpp"
#include "Engine/Model/TileCollectionLoader.hpp"
#include "Core/FileStreamBuf.hpp"
#include "Core/StreamBufRapidJson.hpp"

#include "cinek/allocator.hpp"

#include <random>

#include "Game/Messages/SimMessageClassIds.hpp"
#include "Game/Messages/CreateEntityRequest.hpp"
#include "Game/Messages/CreateEntityResponse.hpp"
#include "Game/Messages/MoveEntityRequest.hpp"
#include "Game/Events/SimEventClassIds.hpp"
#include "Game/Events/CreateEntityEvent.hpp"
#include "Game/Events/MoveEntityEvent.hpp"


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
        _simCommandQueue(128, _allocator),
        _simResponseQueue(512, _allocator),
        _simMessageDispatcher(64, 32, 32, _allocator),
        _sprites(1024, _allocator),
        _simDebugMessages(_allocator),
        _entitySpritePtrs(_allocator),
        _playerEntityId(0)
    {
        //  this should be an application-wide op (move _fontLibrary to ApplicationController)
        _fontLibrary.loadFont(glx::kFontHandle_Default, "static/fonts/DroidSans.ttf", 16);

        //  load game document
        overview::GameTemplates::InitParams initParams;
        initParams.gameDefinitionPath = "game.json";
        initParams.spriteLimit = 256;

        _gameTemplates = allocate_unique<overview::GameTemplates>(_allocator, initParams, _allocator);

        _gameTemplates->loadTileCollection("tiles_dungeon.json", _bitmapLibrary);
        _gameTemplates->loadSpriteCollection("sprites_core.json", _bitmapLibrary);
        _gameTemplates->loadEntityTemplateCollection("entity.json");


        GenerateMapParams generateMapParams;
        generateMapParams.blocksPathname = "blocks.json";
        generateMapParams.floorX = 3;
        generateMapParams.floorY = 3;
        generateMapParams.overlayToFloorRatio = 4;
        generateMapParams.roomLimit = 8;

        _staticWorldMap = generateMapFromTemplates(*_gameTemplates, generateMapParams);

        _stage = allocate_unique<overview::Stage>(_allocator, *_gameTemplates, *_staticWorldMap);
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
        simParams.allocator = _allocator;
        simParams.staticWorldMap = _staticWorldMap.get();
        _simulation = std::move(generateSimulation(*_gameTemplates, simParams));
        
        CreateEntityRequest createReq("avatar", _viewPos, kObjectRefDir);
        _simMessageDispatcher.queue(_simCommandQueue, SimCommand::kCreateEntity, createReq,
            [this](const SDO* data, Message::SequenceId, void*) {
                auto resp = sdo_cast<const CreateEntityResponse*>(data);
                if (resp->responseCode() == CommandResponse::kSuccess)
                {
                    _playerEntityId = resp->entityId();
                }
            });
        
        _simMessageDispatcher.on(SimEvent::kCreateEntity,
            [this](const SDO* data, Message::SequenceId, void* context)
            {
                GameContext& gameContext = *reinterpret_cast<GameContext*>(context);
                auto event = sdo_cast<const CreateEntityEvent*>(data);
                auto entity = _simulation->entity(event->entityId());
                if (entity)
                {
                    auto sprite = allocateSprite(entity->sourceTemplate().spriteName());
                    _entitySpritePtrs.emplace(entity->id(), sprite);
                    auto body = entity->body();
                    applyObjectStateToSprite(sprite, body->transform(), gameContext.timeMs);
                    _stage->attachSpriteInstance(sprite);
                }
            });
        _simMessageDispatcher.on(SimEvent::kMoveEntity,
            [this](const SDO* data, Message::SequenceId, void* context)
            {
                GameContext& gameContext = *reinterpret_cast<GameContext*>(context);
                auto event = sdo_cast<const MoveEntityEvent*>(data);
                auto entity = _simulation->entity(event->entityId());
                if (entity)
                {
                    auto it = _entitySpritePtrs.find(event->entityId());
                    if (it != _entitySpritePtrs.end())
                    {
                        applyObjectStateToSprite(it->second, event->transform(), gameContext.timeMs);
                    }
                }
            });
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
        GameContext context;
        context.timeMs = ticks;
        _simMessageDispatcher.dispatch(_simResponseQueue, ticks, &context);
        _simulation->syncDebugMessages(_simDebugMessages);
        
        
        // update systems (at some point these could be parallel jobs, so
        // it doesn't matter what order thse updates are called)
        _isoScene->update(ticks, _viewPos);
        _simulation->update(_simCommandQueue, _simResponseQueue, ticks);

    }
    
    /*
    void GameView::entitySimResult(const EntityStateEvent* result, uint32_t timeMs)
    {
        if (result->state() == EntityStateEvent::kActivated)
        {
            auto entity = _simulation->entity(result->id());
            if (entity)
            {
                auto sprite = allocateSprite(entity->sourceTemplate().spriteName());
                _entitySpritePtrs.emplace(result->id(), sprite);
                auto body = entity->body();
                applyObjectStateToSprite(sprite, *body, timeMs);
                _stage->attachSpriteInstance(sprite);
            }
        }
        if (result->state() == EntityStateEvent::kDeactivated)
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
    */
    
    SpriteInstancePtr GameView::allocateSprite(const std::string& spriteClassName)
    {
        auto& spriteClass = _gameTemplates->spriteCollection().find(spriteClassName);
        SpriteInstancePtr ptr = _sprites.construct(spriteClass);
        return ptr;
    }
    
    void GameView::freeSprite(SpriteInstancePtr ptr)
    {
        _sprites.destruct(ptr);
    }
    
    void GameView::applyObjectStateToSprite(SpriteInstancePtr sprite,
                                            const WorldObjectTransform& body,
                                            uint32_t timeMs)
    {
        //  direction using (0,1,0) as a reference to South
        //  dir.x determines the side ('west', 'east')
        //  dot product determines direction.
        //
        auto& dir = body.dir;
        float speed = body.speed;
        float dp = glm::dot(dir, kObjectRefDir);
        
        AnimationStateId animId = sprite->state();
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
        
        if (animId != sprite->state())
        {
            sprite->setState(animId, timeMs);
        }
        sprite->setPosition(body.pos);
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
        
        style.lineColor = glx::RGBAColor(192,192,192,255);
        style.lineMethod = glx::kLineMethod_Solid;
        
        for (auto& debugMsg : _simDebugMessages)
        {
            switch (debugMsg.type)
            {
            case SimDebugMessage::kDrawLine:
                {
                    style.lineColor = glx::RGBAColor(debugMsg.color.r*255,
                                                     debugMsg.color.g*255,
                                                     debugMsg.color.b*255,
                                                     255);
                    _graphics.drawLine(_isoScene->isoToScreenPos(debugMsg.p0),
                        _isoScene->isoToScreenPos(debugMsg.p1),
                        style);
                }
                break;
            default:
                break;
            }
        }
        
        style.textColor = glx::RGBAColor(255,0,255,255);
        style.textFont = glx::kFontHandle_Default;
        style.fillMethod = glx::kFillMethod_SolidFill;
        style.fillColor = glx::RGBAColor(0,0,255,255);
        _graphics.drawText("Welcome to the overview 2D project by Samir Sinha.",
                            20, 160, style);

        /*
        style.lineMethod = glx::kLineMethod_Solid;
        style.lineColor = glx::RGBAColor(0,255,0,255);
        _graphics.drawLine(glm::ivec2(25,25), glm::ivec2(648,648), style);
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
        if (!_playerEntityId)
            return;
        
        glm::vec3 newDir(0,0,0);

        const float kAdjX = 1.0f;
        const float kAdjY = 1.0f;

        switch (keycode)
        {
            case SDLK_e:    newDir.y = -kAdjY; break;
            case SDLK_z:    newDir.y = kAdjY; break;
            case SDLK_q:    newDir.x = -kAdjX; break;
            case SDLK_c:    newDir.x = kAdjX; break;
            case SDLK_w:    newDir.x = -kAdjX; newDir.y = -kAdjY; break;
            case SDLK_x:    newDir.x = kAdjX; newDir.y = kAdjY; break;
            case SDLK_a:    newDir.x = -kAdjX; newDir.y = kAdjY; break;
            case SDLK_d:    newDir.x = kAdjX; newDir.y = -kAdjY; break;
            default:
                break;
        }

        if (newDir.x || newDir.y || newDir.z)
        {
            WorldObjectTransform transform;
            transform.setDirty(WorldObjectTransform::kDirection);
            transform.setDirty(WorldObjectTransform::kSpeed);
            transform.dir = glm::normalize(newDir);
            transform.speed = 4.0f;
            
            MoveEntityRequest req;
            req.setEntityId(_playerEntityId);
            req.setTransform(transform);
            _simMessageDispatcher.queue(_simCommandQueue, SimCommand::kMoveEntity, req);
        }
    }

    void GameView::onKeyUp(SDL_Keycode keycode, uint16_t keymod)
    {
        bool stopMove = false;
        switch (keycode)
        {
            case SDLK_e:    stopMove = true; break;
            case SDLK_z:    stopMove = true; break;
            case SDLK_q:    stopMove = true; break;
            case SDLK_c:    stopMove = true; break;
            case SDLK_w:    stopMove = true; break;
            case SDLK_x:    stopMove = true; break;
            case SDLK_a:    stopMove = true; break;
            case SDLK_d:    stopMove = true; break;
            default:
                break;
        }
        
        if (stopMove)
        {
            WorldObjectTransform transform;
            transform.setDirty(WorldObjectTransform::kSpeed);
            transform.speed = 0.f;
            
            MoveEntityRequest req;
            req.setEntityId(_playerEntityId);
            req.setTransform(transform);
            _simMessageDispatcher.queue(_simCommandQueue, SimCommand::kMoveEntity, req);
        }

    }

    }
}