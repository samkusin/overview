//
//  GameScene.hpp
//  Overview
//
//  Created by Samir Sinha on 12/4/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Scenes_GameScene_hpp
#define Overview_Scenes_GameScene_hpp

#include "Prototype/Scene.hpp"
#include "Prototype/Game/Architect.hpp"

#include "Engine/Component/Window/WindowComponentCLI.hpp"
#include "Engine/Model/TileDatabase.hpp"
#include "Engine/Model/SpriteDatabase.hpp"

#include "Graphics/BitmapLibrary.hpp"
#include "Graphics/FontLibrary.hpp"

#include "cinek/allocator.hpp"

namespace cinekine {
    namespace glx {
        class RendererCLI;
    }
    namespace ovengine {
        class Stage;
        class WindowComponentCLI;
        class WindowEventListener;
    }
}

namespace cinekine {
    namespace prototype {

    class SceneController;
    class GameView;

    class GameScene :
        public Scene,
        public ovengine::WindowEventListener
    {
    public:
        GameScene(SceneController& sceneController);

        virtual void update();

    protected:
        virtual void onKeyDown(SDL_Keycode keycode, uint16_t keymod);
        virtual void onKeyUp(SDL_Keycode keycode, uint16_t keymod);

    private:
        void loadTileDatabase(const char* filename);
        void loadSpriteDatabase(const char* filename);
    private:
        Allocator _allocator;
        SceneController& _sceneController;
        ovengine::WindowComponentCLI& _ui;
        
        glx::RendererCLI& _renderer;
        glx::BitmapLibrary _bitmapLibrary;
        glx::FontLibrary _fontLibrary;
        
        ovengine::TileDatabase _tileDb;
        ovengine::SpriteDatabase _spriteDb;
        
        std::shared_ptr<ovengine::Stage> _stage;
        unique_ptr<Architect> _architect;
        glm::vec3 _viewPos;

        std::shared_ptr<GameView> _gameView;

        ovengine::WindowPtr _window;
    };

    }   // namespace ovengine
}   // namespace cinekine


#endif /* defined(Overview_Scenes_GameScene_hpp) */
