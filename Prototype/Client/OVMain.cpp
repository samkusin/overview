//
//  OVMain.cpp
//  Overview
//
//  Created by Samir Sinha on 4/3/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "MessageDispatcher.hpp"

#include "GameStage/GameStage.hpp"

#include <cinek/file.hpp>
#include <SDL2/SDL.h>
#include <bgfx/bgfxplatform.h>
#include <bgfx/bgfx.h>

namespace cinek { namespace overview {

enum
{
    kPollSDLEvent_Quit = 0x0001
};


static uint32_t PollSDLEvents()
{
    uint32_t flags = 0;
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            flags |= kPollSDLEvent_Quit;
            break;
        }
    }
    
    return flags;
}

void run(SDL_Window* window)
{
    Allocator allocator;
    
    int viewWidth = 0;
    int viewHeight = 0;
    
    SDL_GetWindowSize(window, &viewWidth, &viewHeight);

    bgfx::reset(viewWidth, viewHeight, BGFX_RESET_VSYNC);
    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        0x333333ff,
        1.0f,
        0);
    
    //  application
    //
    overview::RenderResources renderResources(
        {
            gfx::createVertexDefinitions(),
            128
        },
        allocator
    );
    
    renderResources.shaders.loadProgram(0x00000001, "Shaders/vs_cubes.bin", "Shaders/fs_cubes.bin");
    
    auto stage = allocate_unique<ovproto::GameStage>(allocator);
    
    //  main loop
    //
    MessageDispatcher dispatcher;
    MessageBuffer msgBuffer(32*1024);
    
    while (true)
    {
        uint32_t sdlEvents = PollSDLEvents();
        if (sdlEvents & kPollSDLEvent_Quit)
            break;
        
        bgfx::setViewRect(0, 0, 0, viewWidth, viewHeight);
        
        //  update stage
        msgBuffer = stage->update(std::move(msgBuffer), renderResources);

        MessageStream msgStream(msgBuffer);
        dispatcher.dispatch(msgStream);
        
        bgfx::submit(0);
        bgfx::frame();
    }
    
    stage = nullptr;
}

} /* namespace overview */ } /* namespace cinek */


int OverviewMain(SDL_Window* window, int argc, char* argv[])
{
    cinek::file::setOpsStdio();
    
    //  renderer initialization
    //
    bgfx::sdlSetWindow(window);
    bgfx::init();
    
    cinek::overview::run(window);
    
    //  subsystem termination
    //
    bgfx::shutdown();
    
    return 0;
}
