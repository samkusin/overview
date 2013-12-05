/*
 *  Main.cpp
 *  Overview
 *
 *  Created by Samir Sinha on 7/24/13.
 *  Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "Debug.hpp"
#include "Director.hpp"
#include "View.hpp"
#include "Theater.hpp"
#include "SpriteDatabaseLoader.hpp"
#include "TileDatabaseLoader.hpp"

#include "Graphics/SDL/SDLRenderer.hpp"
#include "Graphics/GL/GL3Renderer.hpp"

#include "Graphics/Graphics2D.hpp"
#include "Graphics/BitmapLibrary.hpp"
#include "Graphics/FontLibrary.hpp"

#include "UIComponent.hpp"

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

    glx::BitmapLibrary bitmapLibrary(renderer);
    glx::FontLibrary fontLibrary(renderer, 10);
    glx::Graphics2D(renderer, bitmapLibrary, fontLibrary);

    if (!fontLibrary.loadFont(glx::kFontHandle_Default, "static/fonts/DroidSans.ttf", 16))
    {
        OVENGINE_LOG_ERROR("Failed to load system font.");
        return 1;
    }

    //  Startup the UI system
    //  
    ovengine::UIComponent ui(renderer, allocator);
    if (!ui)
    {
        OVENGINE_LOG_ERROR("Failed to initialize UI system");
        return 1;
    }

    //  Startup the Theater MODEL
    ovengine::Theater theater(allocator);
            
    //  Setup Theater <-> Renderer interaction (Director <-> View by proxy)
    //
    //  handle sprite -> bitmap loading.
    theater.getSpriteDatabaseLoader().onBitmapAtlasRequest([&bitmapLibrary]
        (const char* atlasName) -> cinek_bitmap_atlas
        {
            return bitmapLibrary.loadAtlas(atlasName);
        }
    );
    
    theater.getSpriteDatabaseLoader().onBitmapIndexRequest([&bitmapLibrary]
        ( cinek_bitmap_atlas atlas, const char* name) -> cinek_bitmap_index
        {
             const glx::BitmapAtlas* bitmapAtlas = bitmapLibrary.getAtlas(atlas).get();
             if (!bitmapAtlas)
                 return kCinekBitmapIndex_Invalid;
             return bitmapAtlas->getBitmapIndex(name);
        }
    );
    //  handle tile -> bitmap loading.
    theater.getTileDatabaseLoader().onBitmapAtlasRequest([&bitmapLibrary]
        (const char* atlasName) -> cinek_bitmap_atlas
        {
            return bitmapLibrary.loadAtlas(atlasName);
        }
    );
    theater.getTileDatabaseLoader().onBitmapIndexRequest([&bitmapLibrary]
        (cinek_bitmap_atlas atlas, const char* name) -> cinek_bitmap_index
        {
           const glx::BitmapAtlas* bitmapAtlas = bitmapLibrary.getAtlas(atlas).get();
           if (!bitmapAtlas)
               return kCinekBitmapIndex_Invalid;
           return bitmapAtlas->getBitmapIndex(name);
        }
    );

    
    //  startup the simulation specific VIEW.
    ovengine::ViewComponents viewComponents;
    viewComponents.fontLibrary = &fontLibrary;
    viewComponents.bitmapLibrary = &bitmapLibrary;
    ovengine::View* view = cinekine::ovengine::CreateView(theater, renderer, viewComponents);

    //  handle viewpoint sets by Theater MODEL to VIEW interaction
    theater.onViewMapSet([&view]
        (std::shared_ptr<overview::Map>& map, const cinek_ov_pos& pos)
        {
            if (view)
            {
                view->onMapSet(map, pos);
            }
        }
    );
    theater.onViewPosSet([&view]
        (const cinek_ov_pos& pos)
        {
            if (view)
            {
                view->onMapSetPosition(pos);
            }
        }
    );

    //  Startup the Director CONTROLLER script (controls program flow )
    ovengine::Director* director = cinekine::ovengine::CreateDirector(theater.getClient(), ui);

    //  initialize a timer for the main loop
    cinek_timer systemTimer = cinek_timer_create(32);
    uint32_t systemTicks = SDL_GetTicks();

    //  main loop
    bool active = true;
    while (active)
    {
        cinek_time currentTime = cinek_timer_get_time(systemTimer);

        ui.update(currentTime);

        //  event dispatch
        SDL_Event e;
        while (SDL_PollEvent(&e)) 
        {
            if (e.type == SDL_QUIT)
            {
                active = false;
                continue;
            }

            ui.handleInput(e);
        }
        
        ///////////////////////////////////////////////////////////////////
        director->update();

        ///////////////////////////////////////////////////////////////////
        renderer.begin();
    
        renderer.clear(glx::RGBAColor(0,0,0,255));    
        view->render();
        ui.render();

        renderer.display();

        //  update timers for the next frame's time values
        cinek_timer_update(systemTimer, SDL_GetTicks() - systemTicks);
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
