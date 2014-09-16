//
//  GameView.hpp
//  Overview
//
//  Created by Samir Sinha on 8/26/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_GameView_hpp
#define Overview_GameView_hpp

#include "Engine/View.hpp"
#include "Engine/Model/World.hpp"
#include "Engine/Model/Sprite.hpp"
#include "Engine/Model/TileLibrary.hpp"
#include "Engine/Model/SpriteLibrary.hpp"
#include "Graphics/Graphics2D.hpp"
#include "Graphics/RenderTypes.hpp"
#include "Graphics/BitmapLibrary.hpp"
#include "Graphics/FontLibrary.hpp"
#include "Core/JsonUtilities.hpp"

#include "cinek/allocator.hpp"

#include <array>


namespace cinekine {
    namespace ovengine {
        class IsoScene;
    }
}

namespace cinekine {
    namespace prototype {

    class ApplicationController;
    class StageGenerator;
        
    class GameView: public ovengine::View
    {
    public:
        GameView(ApplicationController& application,
                 const Allocator& allocator);
        virtual ~GameView();
        
        virtual void update();

        virtual void render();

        virtual void onMouseButtonDown(MouseButton button, int32_t x, int32_t y);
        virtual void onMouseButtonUp(MouseButton button, int32_t x, int32_t y);
        virtual void onMouseMove(MouseRegion region, int32_t x, int32_t y);
        virtual void onKeyDown(SDL_Keycode keycode, uint16_t keymod);
        virtual void onKeyUp(SDL_Keycode keycode, uint16_t keymod);
        
    private:
        void loadTileCollection(const char* filename);
        void loadSpriteCollection(const char* filename);

    private:
        void renderReset();
        void renderBitmap(const glx::Texture& texture, const glx::BitmapInfo& bitmap,
                          int32_t sx, int32_t sy);
        
    private:
        ApplicationController& _application;
        Allocator _allocator;
        glx::BitmapLibrary _bitmapLibrary;
        glx::FontLibrary _fontLibrary;
        glx::Graphics2D _graphics;
        
        ovengine::TileLibrary _tileLibrary;
        ovengine::SpriteLibrary _spriteLibrary;
        
        JsonDocument _gameDocument;
        
        std::shared_ptr<ovengine::World> _world;
        
        glm::vec3 _viewPos;
        unique_ptr<ovengine::IsoScene> _isoScene;
        
        //  Used for generating stage maps (temporary)
        unique_ptr<StageGenerator> _stageGenerator;
    };

    }
}

#endif
