//
//  GameView.hpp
//  Overview
//
//  Created by Samir Sinha on 8/26/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_GameView_hpp
#define Overview_GameView_hpp

#include "Render/RenderDefines.hpp"

#include "Shared/GameTemplates.hpp"
#include "View/Stage.hpp"
#include "Shared/GameTypes.hpp"

#include "Engine/View.hpp"
#include "Engine/Model/Sprite.hpp"
#include "Engine/Model/TileLibrary.hpp"
#include "Engine/Model/SpriteCollection.hpp"
#include "Engine/Model/ModelTypes.hpp"
#include "Graphics/Graphics2D.hpp"
#include "Graphics/RenderTypes.hpp"
#include "Graphics/BitmapLibrary.hpp"
#include "Graphics/FontLibrary.hpp"
#include "Core/JsonUtilities.hpp"
#include "Core/MessageQueue.hpp"
#include "Core/MessageDispatcher.hpp"

#include "cinek/allocator.hpp"
#include "cinek/map.hpp"

#include <array>


namespace cinek {
    class EventBase;
    
    namespace overview {
        class EntityStateEvent;
        class IsoScene;
        class WorldObject;
        class StaticWorldMap;
    }
}

namespace cinek {
    namespace overview {

    class Simulation;

    //  Animation states as defined in the sprite database file
    //
    enum
    {
        kAnimationState_Idle            = 0,
        kAnimationState_Idle_North      = 1,
        kAnimationState_Idle_NorthEast  = 2,
        kAnimationState_Idle_East       = 3,
        kAnimationState_Idle_SouthEast  = 4,
        kAnimationState_Idle_South      = 5,
        kAnimationState_Idle_SouthWest  = 6,
        kAnimationState_Idle_West       = 7,
        kAnimationState_Idle_NorthWest  = 8,
        kAnimationState_Walk_North      = 9,
        kAnimationState_Walk_NorthEast  = 10,
        kAnimationState_Walk_East       = 11,
        kAnimationState_Walk_SouthEast  = 12,
        kAnimationState_Walk_South      = 13,
        kAnimationState_Walk_SouthWest  = 14,
        kAnimationState_Walk_West       = 15,
        kAnimationState_Walk_NorthWest  = 16
    };

    class ApplicationController;

    class GameView: public overview::View
    {
    public:
        GameView(ApplicationController& application,
                 const Allocator& allocator);
        virtual ~GameView();

        virtual void update(uint32_t ticks);

        virtual void render();

        virtual void onMouseButtonDown(MouseButton button, int32_t x, int32_t y);
        virtual void onMouseButtonUp(MouseButton button, int32_t x, int32_t y);
        virtual void onMouseMove(MouseRegion region, int32_t x, int32_t y);
        virtual void onKeyDown(SDL_Keycode keycode, uint16_t keymod);
        virtual void onKeyUp(SDL_Keycode keycode, uint16_t keymod);

    private:
        void renderReset();
        void renderBitmap(const glx::Texture& texture, const glx::BitmapInfo& bitmap,
                          int32_t sx, int32_t sy);

        void handleSimResults(const EventBase* evt, uint32_t timeMs);
        
        void entitySimResult(const EntityStateEvent* result, uint32_t timeMs);
        
        SpriteInstancePtr allocateSprite(const std::string& spriteClassName);
        void freeSprite(SpriteInstancePtr ptr);
        
        void applyObjectStateToSprite(SpriteInstancePtr sprite,
                        const WorldObjectTransform& body,
                        uint32_t timeMs);
        
    private:
        ApplicationController& _application;
        Allocator _allocator;
        glx::BitmapLibrary _bitmapLibrary;
        glx::FontLibrary _fontLibrary;
        glx::Graphics2D _graphics;

        unique_ptr<overview::GameTemplates> _gameTemplates;
        unique_ptr<StaticWorldMap> _staticWorldMap;
        unique_ptr<Simulation> _simulation;
        
        MessageQueue _simCommandQueue;
        MessageQueue _simResponseQueue;
        MessageDispatcher _simMessageDispatcher;
        
        vector<SimDebugMessage> _simDebugMessages;
        
        ObjectPool<SpriteInstance> _sprites;
        unordered_map<EntityId, SpriteInstancePtr> _entitySpritePtrs;
        std::shared_ptr<overview::Stage> _stage;
        unique_ptr<overview::IsoScene> _isoScene;

        Point _viewPos;
        EntityId _playerEntityId;
        
    private:
        struct GameContext
        {
            uint32_t timeMs;
        };
    };

    }
}

#endif
