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
    ovengine::Renderer renderer(theater, rendererInitParams, window, allocator);
    
    if (renderer &&
        renderer.getFontLibrary().loadFont(ovengine::kFontHandle_Default, "DroidSans.ttf", 16))
    {
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
    }
    else
    {
        OVENGINE_LOG_ERROR("Failed to initialize renderer.");
        return 1;
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
