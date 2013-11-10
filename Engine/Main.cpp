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
#include "Render/SDL/SDLRenderer.hpp"
#include "Render/BitmapLibrary.hpp"
#include "Render/FontLibrary.hpp"
#include "Theater.hpp"
#include "SpriteDatabaseLoader.hpp"
#include "TileDatabaseLoader.hpp"

#include "UI/IMGUIGfx.hpp"
#include "UI/IMGUIFrame.hpp"
#include "IMGUIView.hpp"

#include "cinek/cpp/ckalloc.hpp"

#include "SDL2/SDL.h"

using namespace cinekine;

#ifdef __cplusplus
extern "C" {
#endif

int OverviewSDLMain(SDL_Window* window, int argc, char* argv[])
{    
    Allocator allocator;

    ovengine::Renderer::InitParameters rendererInitParams;
    ovengine::SDLRenderer renderer(rendererInitParams, window, allocator);

    if (!renderer)
    {
        OVENGINE_LOG_ERROR("Failed to initialize renderer.");
        return 1;
    }
    if (!renderer.getFontLibrary().loadFont(ovengine::kFontHandle_Default, "DroidSans.ttf", 16))
    {
        OVENGINE_LOG_ERROR("Failed to load system font.");
        return 1;
    }

    ovengine::Theater theater(allocator);
            
    //  Setup Theater <-> Renderer interaction (Director <-> View by proxy)
    //
    //  handle sprite -> bitmap loading.
    theater.getSpriteDatabaseLoader().onBitmapAtlasRequest([&renderer]
        (const char* atlasName) -> cinek_bitmap_atlas
        {
            return renderer.getBitmapLibrary().loadAtlas(atlasName);
        }
    );
    
    theater.getSpriteDatabaseLoader().onBitmapIndexRequest([&renderer]
        ( cinek_bitmap_atlas atlas, const char* name) -> cinek_bitmap_index
        {
             const ovengine::BitmapAtlas* bitmapAtlas = renderer.getBitmapLibrary().getAtlas(atlas);
             if (!bitmapAtlas)
                 return kCinekBitmapIndex_Invalid;
             return bitmapAtlas->getBitmapIndex(name);
        }
    );
    //  handle tile -> bitmap loading.
    theater.getTileDatabaseLoader().onBitmapAtlasRequest([&renderer]
        (const char* atlasName) -> cinek_bitmap_atlas
        {
            return renderer.getBitmapLibrary().loadAtlas(atlasName);
        }
    );
    theater.getTileDatabaseLoader().onBitmapIndexRequest([&renderer]
        (cinek_bitmap_atlas atlas, const char* name) -> cinek_bitmap_index
        {
           const ovengine::BitmapAtlas* bitmapAtlas = renderer.getBitmapLibrary().getAtlas(atlas);
           if (!bitmapAtlas)
               return kCinekBitmapIndex_Invalid;
           return bitmapAtlas->getBitmapIndex(name);
        }
    );
   
    //  startup the simulation specific View.
    ovengine::View* view = cinekine::ovengine::CreateView(theater, renderer);
    
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

    //  IMGUI view
    cinekine::ovengine::IMGUIView imguiView(renderer);
    imgui::Style imguiStyle;
    imgui::Allocator imguiAllocator;
    imgui::Gfx imguiGfx(512, 16*1024, imguiAllocator);
    imgui::Frame imguiFrame(imguiGfx, { 0, 0, 1024, 768 }, imguiStyle, imguiAllocator);

    //  startup the simulation specific Director.
    ovengine::Director* director = cinekine::ovengine::CreateDirector(theater.getClient(),
                                                                      imguiFrame);
    //  main loop
    bool active = true;
    while (active)
    {
        //  event dispatch
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                active = false;
                continue;
            }
        }
        
        ///////////////////////////////////////////////////////////////////
        imgui::Input imguiInput;
        imguiGfx.clearCommandQueue();
        imguiFrame.begin(imguiInput);
        
        director->update();

        imguiFrame.end();
        ///////////////////////////////////////////////////////////////////
        renderer.begin();
        
        view->render();
        imguiView.render(imguiGfx.getCommandQueue());
        
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
