//
//  OVMain.cpp
//  Overview
//
//  Created by Samir Sinha on 4/3/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "ClientController.hpp"
#include "MessageDispatcher.hpp"

#include <SDL2/SDL.h>
#include <bgfx/bgfxplatform.h>
#include <bgfx/bgfx.h>

using namespace cinek;

namespace cinek { namespace ovproto {

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

} /* namespace ovproto */ } /* namespace cinek */


int OverviewMain(SDL_Window* window, int argc, char* argv[])
{
    int viewWidth = 0;
    int viewHeight = 0;
    
    SDL_GetWindowSize(window, &viewWidth, &viewHeight);
    
    //  renderer initialization
    //
    bgfx::sdlSetWindow(window);
    bgfx::init();
    
    bgfx::reset(viewWidth, viewHeight, BGFX_RESET_VSYNC);
    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        0x333333ff,
        1.0f,
        0);
    
    overview::MessageDispatcher dispatcher;
    overview::MessageBuffer msgBuffer(32*1024);
    
    while (true)
    {
        uint32_t sdlEvents = ovproto::PollSDLEvents();
        if (sdlEvents & ovproto::kPollSDLEvent_Quit)
            break;
        
        bgfx::setViewRect(0, 0, 0, viewWidth, viewHeight);
        bgfx::submit(0);
        
        msgBuffer = cinek::ovproto::run(std::move(msgBuffer));
        
        overview::MessageStream msgStream(msgBuffer);
        dispatcher.dispatch(msgStream);
        
        bgfx::frame();
    }
    
    //  subsystem termination
    bgfx::shutdown();
    
    return 0;

}
