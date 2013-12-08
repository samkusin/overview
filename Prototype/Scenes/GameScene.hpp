//
//  GameScene.hpp
//  Overview
//
//  Created by Samir Sinha on 12/4/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Scenes_GameScene_hpp
#define Overview_Scenes_GameScene_hpp

#include "Scene.hpp"
#include "Engine/Component/Window/WindowComponentCLI.hpp"
#include "Graphics/BitmapLibrary.hpp"
#include "Graphics/FontLibrary.hpp"

#include "cinek/overview/stage.hpp"
#include "cinek/cpp/ckalloc.hpp"

namespace cinekine {
    namespace glx {
        class RendererCLI;
    }
    namespace ovengine {
        class TheaterCLI;
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
        Allocator _allocator;
        SceneController& _sceneController;
        ovengine::TheaterCLI& _theater;
        ovengine::WindowComponentCLI& _ui;
        glx::RendererCLI& _renderer;

        glx::BitmapLibrary _bitmapLibrary;
        glx::FontLibrary _fontLibrary;

        overview::Stage _stage;
        cinek_ov_pos _viewPos;

        std::shared_ptr<GameView> _gameView;

        ovengine::WindowPtr _window;
    };
        
    }   // namespace ovengine
}   // namespace cinekine


#endif /* defined(Overview_Scenes_GameScene_hpp) */
