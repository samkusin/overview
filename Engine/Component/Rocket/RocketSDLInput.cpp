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

    RocketSDLInput::RocketSDLInput() :
        _keyMap()
    {
        using namespace Rocket::Core;
        _keyMap[SDLK_BACKSPACE]     = Input::KI_BACK; 
        _keyMap[SDLK_TAB]           = Input::KI_TAB;
        _keyMap[SDLK_RETURN]        = Input::KI_RETURN;
        _keyMap[SDLK_ESCAPE]        = Input::KI_ESCAPE;
        _keyMap[SDLK_SPACE]         = Input::KI_SPACE;
        _keyMap[SDLK_QUOTE]         = Input::KI_OEM_7;
        _keyMap[SDLK_COMMA]         = Input::KI_OEM_COMMA;
        _keyMap[SDLK_MINUS]         = Input::KI_OEM_MINUS;
        _keyMap[SDLK_PERIOD]        = Input::KI_OEM_PERIOD;
        _keyMap[SDLK_SLASH]         = Input::KI_OEM_2;
        _keyMap[SDLK_0]             = Input::KI_0;
        _keyMap[SDLK_1]             = Input::KI_1;
        _keyMap[SDLK_2]             = Input::KI_2;
        _keyMap[SDLK_3]             = Input::KI_3;
        _keyMap[SDLK_4]             = Input::KI_4;
        _keyMap[SDLK_5]             = Input::KI_5;
        _keyMap[SDLK_6]             = Input::KI_6;
        _keyMap[SDLK_7]             = Input::KI_7;
        _keyMap[SDLK_8]             = Input::KI_8;
        _keyMap[SDLK_9]             = Input::KI_9;
        _keyMap[SDLK_SEMICOLON]     = Input::KI_OEM_1;
        _keyMap[SDLK_EQUALS]        = Input::KI_OEM_PLUS;
        _keyMap[SDLK_LEFTBRACKET]   = Input::KI_OEM_4;
        _keyMap[SDLK_BACKSLASH]     = Input::KI_OEM_5;
        _keyMap[SDLK_RIGHTBRACKET]  = Input::KI_OEM_6;
        _keyMap[SDLK_BACKQUOTE]     = Input::KI_OEM_3;
        _keyMap[SDLK_a]             = Input::KI_A;
        _keyMap[SDLK_b]             = Input::KI_B;
        _keyMap[SDLK_c]             = Input::KI_C;
        _keyMap[SDLK_d]             = Input::KI_D;
        _keyMap[SDLK_e]             = Input::KI_E;
        _keyMap[SDLK_f]             = Input::KI_F;
        _keyMap[SDLK_g]             = Input::KI_G;
        _keyMap[SDLK_h]             = Input::KI_H;
        _keyMap[SDLK_i]             = Input::KI_I;
        _keyMap[SDLK_j]             = Input::KI_J;
        _keyMap[SDLK_k]             = Input::KI_K;
        _keyMap[SDLK_l]             = Input::KI_L;
        _keyMap[SDLK_m]             = Input::KI_M;
        _keyMap[SDLK_n]             = Input::KI_N;
        _keyMap[SDLK_o]             = Input::KI_O;
        _keyMap[SDLK_p]             = Input::KI_P;
        _keyMap[SDLK_q]             = Input::KI_Q;
        _keyMap[SDLK_r]             = Input::KI_R;
        _keyMap[SDLK_s]             = Input::KI_S;
        _keyMap[SDLK_t]             = Input::KI_T;
        _keyMap[SDLK_u]             = Input::KI_U;
        _keyMap[SDLK_v]             = Input::KI_V;
        _keyMap[SDLK_w]             = Input::KI_W;
        _keyMap[SDLK_x]             = Input::KI_X;
        _keyMap[SDLK_y]             = Input::KI_Y;
        _keyMap[SDLK_z]             = Input::KI_Z;
        _keyMap[SDLK_DELETE]        = Input::KI_DELETE;
        _keyMap[SDLK_CAPSLOCK]      = Input::KI_CAPITAL;
        _keyMap[SDLK_F1]            = Input::KI_F1;
        _keyMap[SDLK_F2]            = Input::KI_F2;
        _keyMap[SDLK_F3]            = Input::KI_F3;
        _keyMap[SDLK_F4]            = Input::KI_F4;
        _keyMap[SDLK_F5]            = Input::KI_F5;
        _keyMap[SDLK_F6]            = Input::KI_F6;
        _keyMap[SDLK_F7]            = Input::KI_F7;
        _keyMap[SDLK_F8]            = Input::KI_F8;
        _keyMap[SDLK_F9]            = Input::KI_F9;
        _keyMap[SDLK_F10]           = Input::KI_F10;
        _keyMap[SDLK_F11]           = Input::KI_F11;
        _keyMap[SDLK_F12]           = Input::KI_F12;
        _keyMap[SDLK_PRINTSCREEN]   = Input::KI_SNAPSHOT;
        _keyMap[SDLK_SCROLLLOCK]    = Input::KI_SCROLL;
        _keyMap[SDLK_PAUSE]         = Input::KI_PAUSE;
        _keyMap[SDLK_INSERT]        = Input::KI_INSERT;
        _keyMap[SDLK_HOME]          = Input::KI_HOME;
        _keyMap[SDLK_PAGEUP]        = Input::KI_PRIOR;
        _keyMap[SDLK_END]           = Input::KI_END;
        _keyMap[SDLK_PAGEDOWN]      = Input::KI_NEXT;
        _keyMap[SDLK_RIGHT]         = Input::KI_RIGHT;
        _keyMap[SDLK_LEFT]          = Input::KI_LEFT;
        _keyMap[SDLK_DOWN]          = Input::KI_DOWN;
        _keyMap[SDLK_UP]            = Input::KI_UP;
        _keyMap[SDLK_NUMLOCKCLEAR]  = Input::KI_NUMLOCK;
        _keyMap[SDLK_KP_DIVIDE]     = Input::KI_DIVIDE;
        _keyMap[SDLK_KP_MULTIPLY]   = Input::KI_MULTIPLY;
        _keyMap[SDLK_KP_MINUS]      = Input::KI_SUBTRACT;
        _keyMap[SDLK_KP_PLUS]       = Input::KI_ADD;
        _keyMap[SDLK_KP_ENTER]      = Input::KI_NUMPADENTER;
        _keyMap[SDLK_KP_1]          = Input::KI_NUMPAD1;
        _keyMap[SDLK_KP_2]          = Input::KI_NUMPAD2;
        _keyMap[SDLK_KP_3]          = Input::KI_NUMPAD3;
        _keyMap[SDLK_KP_4]          = Input::KI_NUMPAD4;
        _keyMap[SDLK_KP_5]          = Input::KI_NUMPAD5;
        _keyMap[SDLK_KP_6]          = Input::KI_NUMPAD6;
        _keyMap[SDLK_KP_7]          = Input::KI_NUMPAD7;
        _keyMap[SDLK_KP_8]          = Input::KI_NUMPAD8;
        _keyMap[SDLK_KP_9]          = Input::KI_NUMPAD9;
        _keyMap[SDLK_KP_0]          = Input::KI_NUMPAD0;
        _keyMap[SDLK_KP_PERIOD]     = Input::KI_DECIMAL;
        _keyMap[SDLK_F13]           = Input::KI_F13;
        _keyMap[SDLK_F14]           = Input::KI_F14;
        _keyMap[SDLK_F15]           = Input::KI_F15;
        _keyMap[SDLK_F16]           = Input::KI_F16;
        _keyMap[SDLK_F17]           = Input::KI_F17;
        _keyMap[SDLK_F18]           = Input::KI_F18;
        _keyMap[SDLK_F19]           = Input::KI_F19;
        _keyMap[SDLK_F20]           = Input::KI_F20;
        _keyMap[SDLK_F21]           = Input::KI_F21;
        _keyMap[SDLK_F22]           = Input::KI_F22;
        _keyMap[SDLK_F23]           = Input::KI_F23;
        _keyMap[SDLK_F24]           = Input::KI_F24;
        _keyMap[SDLK_CLEAR]         = Input::KI_OEM_CLEAR;
        _keyMap[SDLK_LCTRL]         = Input::KI_LCONTROL;
        _keyMap[SDLK_LSHIFT]        = Input::KI_LSHIFT;
        _keyMap[SDLK_LALT]          = Input::KI_LMENU;
        _keyMap[SDLK_RCTRL]         = Input::KI_RCONTROL;
        _keyMap[SDLK_RSHIFT]        = Input::KI_RSHIFT;
        _keyMap[SDLK_RALT]          = Input::KI_RMENU;
    }

    Rocket::Core::Input::KeyIdentifier RocketSDLInput::translateSDLKey(SDL_Keysym key)
    {
        auto it = _keyMap.find(key.sym);
        if (it == _keyMap.end())
            return Rocket::Core::Input::KI_UNKNOWN;

        return (*it).second;
    }


    bool RocketSDLInput::dispatchSDLEvent(const SDL_Event& event, Rocket::Core::Context* context)
    {
        SDLMod modState = SDL_GetModState();
        int keyMods = 0;

        if((modState & KMOD_SHIFT) == KMOD_SHIFT) {
            keyMods |= Rocket::Core::Input::KM_SHIFT;
        }
        if((modState & KMOD_CTRL) == KMOD_CTRL) {
            keyMods |= Rocket::Core::Input::KM_CTRL;
        }
        
        if((modState & KMOD_ALT) == KMOD_ALT) {
            keyMods |= Rocket::Core::Input::KM_ALT;
        }
        
        if((modState & KMOD_META) == KMOD_META) {
            keyMods |= Rocket::Core::Input::KM_META;
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
