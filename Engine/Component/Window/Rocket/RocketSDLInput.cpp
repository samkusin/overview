/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Cinekine Media
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 */

#include "RocketSDLInput.hpp"

#include "Rocket/Core/Context.h"
#include "Rocket/Core/Input.h"

#include "SDL2/SDL_keyboard.h"


namespace cinekine {
    namespace ovengine {

    RocketSDLInput::RocketSDLInput() 
    {
    }

    bool RocketSDLInput::dispatchSDLEvent(const SDL_Event& event, Rocket::Core::Context* context)
    {
        SDL_Keymod modState = SDL_GetModState();
        int keyMods = 0;

        if(modState & KMOD_SHIFT) {
            keyMods |= Rocket::Core::Input::KM_SHIFT;
        }
        if(modState & KMOD_CTRL) {
            keyMods |= Rocket::Core::Input::KM_CTRL;
        }
        
        if(modState & KMOD_ALT) {
            keyMods |= Rocket::Core::Input::KM_ALT;
        }
               
        if((modState & KMOD_NUM) == KMOD_NUM) {
            keyMods |= Rocket::Core::Input::KM_NUMLOCK;
        }
        
        if((modState & KMOD_CAPS) == KMOD_CAPS) {
            keyMods |= Rocket::Core::Input::KM_CAPSLOCK;
        }
                        
        switch (event.type)
        {
        case SDL_MOUSEMOTION:
            context->ProcessMouseMove(event.motion.x, event.motion.y, keyMods);
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            {
                int buttonIndex = event.button.button == SDL_BUTTON_LEFT ? 0 :
                                  event.button.button == SDL_BUTTON_RIGHT ? 1 :
                                  event.button.button == SDL_BUTTON_MIDDLE ? 2 :
                                  event.button.button == SDL_BUTTON_X1 ? 3 :
                                  event.button.button == SDL_BUTTON_X2 ? 4 :
                                  -1;
                if (buttonIndex < 0)
                    return false;

                if (event.type == SDL_MOUSEBUTTONDOWN)
                    context->ProcessMouseButtonDown(buttonIndex, keyMods);
                else 
                    context->ProcessMouseButtonUp(buttonIndex, keyMods);
            }
            break;
        case SDL_MOUSEWHEEL:
            //  invert wheel (libRocket treats down as positive vs SDL)
            context->ProcessMouseWheel(-event.wheel.y, keyMods);
            break;
        case SDL_KEYDOWN:
            {
                Rocket::Core::Input::KeyIdentifier key = translateSDLKey(event.key.keysym);
                if (!context->ProcessKeyDown(key, keyMods))
                    return false;
            }
            break;
        case SDL_KEYUP:
            {
                Rocket::Core::Input::KeyIdentifier key = translateSDLKey(event.key.keysym);
                if (!context->ProcessKeyUp(key, keyMods))
                    return false;
            }
            break;
        default:
            return false;
        }

        return true;
    }

    }   // namespace ovengine
}   // namespace cinekine
