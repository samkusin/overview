/*
 *  Main.cpp
 *  Overview
 *
 *  Created by Samir Sinha on 7/24/13.
 *  Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "Controller.hpp"
#include "Debug.hpp"
#include "Render/Renderer.hpp"

#include "cinek/cpp/ckalloc.hpp"

#include "SDL.h"

using namespace cinekine;


#ifdef __cplusplus
extern "C" {
#endif

int OverviewSDLMain(int argc, char* argv[])
{    
    SDL_Window* window = SDL_CreateWindow("Overview Engine",
                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               1024, 768,
                               SDL_WINDOW_SHOWN);
    if (!window)
    {
        OVENGINE_LOG_ERROR("SDL Failed to initialize window : %s", SDL_GetError());
        return 1;
    }

    Allocator allocator;
    ovengine::Controller controller(allocator);
    ovengine::Renderer::InitParameters rendererInitParams;
    rendererInitParams.bitmapAtlasDir = "bitmaps";
    ovengine::Renderer renderer(window, rendererInitParams, allocator);

    bool active = true;
       
    while (active)
    {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                active = false;
                continue;
            }
        }
        
        renderer.begin();
        
        renderer.end();
    }
   
    SDL_Quit();
   
    return 0;
}

#ifdef __cplusplus
}
#endif
