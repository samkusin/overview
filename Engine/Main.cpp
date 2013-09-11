/*
 *  Main.cpp
 *  Overview
 *
 *  Created by Samir Sinha on 7/24/13.
 *  Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "Debug.hpp"
#include "Director.hpp"
#include "GameView.hpp"
#include "Render/Renderer.hpp"
#include "Render/BitmapLibrary.hpp"
#include "Theater.hpp"
#include "SpriteDatabaseLoader.hpp"
#include "TileDatabaseLoader.hpp"

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
                               SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window)
    {
        OVENGINE_LOG_ERROR("SDL Failed to initialize window : %s", SDL_GetError());
        return 1;
    }

    Allocator allocator;
   
    ovengine::Theater theater(allocator);
    
    ovengine::Renderer::InitParameters rendererInitParams;
    rendererInitParams.bitmapAtlasDir = "bitmaps";
    ovengine::Renderer renderer(theater, window, rendererInitParams, allocator);
    
    //  startup the simulation specific View.
    ovengine::View* view = cinekine::ovengine::CreateView(renderer);
    
    //  handle viewpoint sets by Theater.
    theater.onViewMapSet([&view] (std::shared_ptr<overview::Map>& map, const cinek_ov_pos& pos)
        {
            if (view)
            {
                view->onMapSet(map, pos);
            }
        }
    );
    theater.onViewPosSet([&view] (const cinek_ov_pos& pos)
        {
            if (view)
            {
                view->onMapSetPosition(pos);
            }
        }
    );
    
    //  startup the simulation specific Director.
    ovengine::Director* director = cinekine::ovengine::CreateDirector(theater.getClient());
    
    
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
        
        director->update();
        
        renderer.begin();
        view->render();
        renderer.end();
    }
    
    ovengine::DestroyDirector(director);
    director = nullptr;
    ovengine::DestroyView(view);
    view = nullptr;

    return 0;
}

#ifdef __cplusplus
}
#endif
