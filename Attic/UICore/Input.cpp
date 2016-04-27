//
//  Input.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/15/16.
//
//

#include "Input.hpp"

#include "UICore/oui.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_mouse.h>

#include <cinek/debug.h>

namespace cinek {
    namespace uicore {

bool InputState::testKey(uint8_t key) const
{
    CK_ASSERT_RETURN_VALUE(key < keystateArraySize, false);
    return keystate[key];
}

bool InputState::testKeyMod(int mod) const
{
    return (keyModifiers & mod) != 0;
}

uint32_t pollSDLEvents(InputState& state)
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
    state.keyModifiers = SDL_GetModState();
    
    return flags;
}



    } /* namespace uicore */
} /* namespace cinek */

