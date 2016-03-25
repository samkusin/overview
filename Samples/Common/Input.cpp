//
//  Input.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/15/16.
//
//

#include "Input.hpp"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_mouse.h>

#include <cinek/debug.h>

namespace cinek {
    namespace input {

bool InputState::testKey(uint8_t key) const
{
    CK_ASSERT_RETURN_VALUE(key < keystateArraySize, false);
    return keystate[key];
}

bool InputState::testKeyMod(int mod) const
{
    return (keyModifiers & mod) != 0;
}

uint32_t processSDLEvent(InputState& state, const SDL_Event& event)
{
    uint32_t flags = 0;
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
        }
        break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        break;
        
    case SDL_TEXTINPUT:
        break;
    default:
        break;
    }
        
    return flags;
}



    } /* namespace uicore */
} /* namespace cinek */

