/*
 *  Main.cpp
 *  Overview
 *
 *  Created by Samir Sinha on 7/24/13.
 *  Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "Debug.hpp"
#include "Director.hpp"

#include "Graphics/SDL/SDLRenderer.hpp"
#include "Graphics/GL/GL3Renderer.hpp"

#include "Graphics/Graphics2D.hpp"
#include "Graphics/BitmapLibrary.hpp"
#include "Graphics/FontLibrary.hpp"

#include "Component/Window/WindowComponent.hpp"

#include "cinek/allocator.hpp"

#include <SDL2/SDL.h>

using namespace cinekine;

#ifdef __cplusplus
extern "C" {
#endif

int OverviewSDLMain(SDL_Window* window, int argc, char* argv[])
{
    Allocator allocator;

    srand((uint32_t)time(NULL));

    glx::RendererInitParameters rendererInitParams;
    glx::GL3Renderer renderer(rendererInitParams, window, allocator);
    //glx::SDLRenderer renderer(rendererInitParams, window, allocator);
    if (!renderer)
    {
        OVENGINE_LOG_ERROR("Failed to initialize renderer.");
        return 1;
    }

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
    ovengine::Director* director =
        cinekine::ovengine::CreateDirector(*windowComponent, renderer);

    //  main loop

    bool active = true;
    while (active)
    {
        uint32_t systemTicks = SDL_GetTicks();
        windowComponent->update(systemTicks);

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
    }

    ovengine::DestroyDirector(director);
    director = nullptr;
    
    windowComponent->update(SDL_GetTicks());

    return 0;
}

#ifdef __cplusplus
}
#endif
