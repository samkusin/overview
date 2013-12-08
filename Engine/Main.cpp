/*
 *  Main.cpp
 *  Overview
 *
 *  Created by Samir Sinha on 7/24/13.
 *  Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "Debug.hpp"
#include "Director.hpp"
#include "Theater.hpp"
#include "View.hpp"

#include "SpriteDatabaseLoader.hpp"
#include "TileDatabaseLoader.hpp"

#include "Graphics/SDL/SDLRenderer.hpp"
#include "Graphics/GL/GL3Renderer.hpp"

#include "Graphics/Graphics2D.hpp"
#include "Graphics/BitmapLibrary.hpp"
#include "Graphics/FontLibrary.hpp"

#include "Component/Window/WindowComponent.hpp"

#include "cinek/cpp/ckalloc.hpp"
#include "cinek/core/cktimer.h"

#include "SDL2/SDL.h"

using namespace cinekine;

#ifdef __cplusplus
extern "C" {
#endif

int OverviewSDLMain(SDL_Window* window, int argc, char* argv[])
{    
    Allocator allocator;

    glx::RendererInitParameters rendererInitParams;
    glx::GL3Renderer renderer(rendererInitParams, window, allocator);
    //glx::SDLRenderer renderer(rendererInitParams, window, allocator);
    if (!renderer)
    {
        OVENGINE_LOG_ERROR("Failed to initialize renderer.");
        return 1;
    }

    //  debug font
    glx::FontLibrary fontLibrary(renderer, 2);
    if (!fontLibrary.loadFont(glx::kFontHandle_Default, "static/fonts/DroidSans.ttf", 16))
    {
        OVENGINE_LOG_ERROR("Failed to load system font.");
        return 1;
    }

    //  Startup the Theater MODEL
    ovengine::Theater theater(allocator);

    //  Startup the UI system
    ovengine::WindowComponentPtr windowComponent =
                                ovengine::createWindowComponent(
                                        "rocket",
                                        renderer,
                                        allocator);
    if (!windowComponent)
    {
        OVENGINE_LOG_ERROR("Failed to initialize UI system");
        return 1;
    }
 
    //  Startup the Director CONTROLLER script (controls program flow )
    ovengine::Director* director = cinekine::ovengine::CreateDirector(theater.getCLI(),
                                            *windowComponent,
                                            renderer);

    //  main loop
    cinek_timer systemTimer = cinek_timer_create(32);
    uint32_t systemTicks = SDL_GetTicks();
    bool active = true;
    while (active)
    {
        cinek_time currentTime = cinek_timer_get_time(systemTimer);

        windowComponent->update(currentTime);

        //  event dispatch
        SDL_Event e;
        while (SDL_PollEvent(&e)) 
        {
            if (e.type == SDL_QUIT)
            {
                active = false;
                continue;
            }

            windowComponent->handleInput(e);
        }
        
        ///////////////////////////////////////////////////////////////////
        director->update();

        ///////////////////////////////////////////////////////////////////
        renderer.begin();
    
        renderer.clear(glx::RGBAColor(0,0,0,255));    
        windowComponent->render();

        renderer.display();

        //  update timers for the next frame's time values
        cinek_timer_update(systemTimer, SDL_GetTicks() - systemTicks);
    }
   
    ovengine::DestroyDirector(director);
    director = nullptr;

    return 0;
}

#ifdef __cplusplus
}
#endif
