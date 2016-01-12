//
//  Common.cpp
//  SampleCommon
//
//  Created by Samir Sinha on 11/8/15.
//  Copyright © 2015 Cinekine. All rights reserved.
//

#include "Common.hpp"

#include "UICore/oui.h"


#include <cinek/file.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_mouse.h>
#include <bgfx/bgfxplatform.h>
#include <bgfx/bgfx.h>



////////////////////////////////////////////////////////////////////////////////

namespace cinek {
    namespace ove {

bool InputState::testKey(uint8_t key) const
{
    CK_ASSERT_RETURN_VALUE(key < keystateArraySize, false);
    return keystate[key];
}

bool InputState::testKeyMod(int mod) const
{
    return (keyModifiers & mod) != 0;
}

    }
}

uint32_t pollSDLEvents(cinek::ove::InputState& state)
{
    int mx, my;
    
    //  handle Mouse UI, which is polled per frame instead of set per event.
    uint32_t mbtn = SDL_GetMouseState(&mx, &my);
    SDL_GetRelativeMouseState(&state.mdx, &state.mdy);
    
    uiSetCursor(mx, my);
    
    uiSetButton(0, 0, (mbtn & SDL_BUTTON_LMASK) != 0);
    uiSetButton(2, 0, (mbtn & SDL_BUTTON_RMASK) != 0);
    
    state.mx = mx;
    state.my = my;
    state.mbtn = mbtn;
    state.mxWheel = 0;
    state.myWheel = 0;
    
    //  poll system and key events
    uint32_t flags = 0;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            flags |= kPollSDLEvent_Quit;
            break;
            
        case SDL_WINDOWEVENT:
            break;
        
        case SDL_MOUSEWHEEL:
            if (event.wheel.which != SDL_TOUCH_MOUSEID) {
                state.mxWheel += event.wheel.x;
                state.myWheel += event.wheel.y;
                uiSetScroll(state.mxWheel, state.myWheel);
            }
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            {
                SDL_KeyboardEvent& kevt = event.key;
                if (!kevt.repeat) {
                    uiSetKey(kevt.keysym.scancode, kevt.keysym.mod, kevt.state);
                }
            }
            break;
            
        case SDL_TEXTINPUT:
            uiSetChar(event.text.text[0]);
            break;
        default:
            break;
        }
    }
    
    state.keystate = SDL_GetKeyboardState(&state.keystateArraySize);
    
    return flags;
}

////////////////////////////////////////////////////////////////////////////////

int OverviewMain(SDL_Window* window, int argc, char* argv[])
{
    cinek::file::setOpsStdio();
    
    int viewWidth;
    int viewHeight;
    
    SDL_GetWindowSize(window, &viewWidth, &viewHeight);

    //  renderer initialization
    //
    bgfx::sdlSetWindow(window);
    bgfx::init();

    bgfx::reset(viewWidth, viewHeight, BGFX_RESET_VSYNC);
    bgfx::setDebug(BGFX_DEBUG_TEXT /*|BGFX_DEBUG_STATS*/ );
    
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        0x001122ff,
        1.0f,
        0);

    int result = runSample(viewWidth, viewHeight);
    
    //  subsystem termination
    //
    bgfx::shutdown();
    
    return result;
}