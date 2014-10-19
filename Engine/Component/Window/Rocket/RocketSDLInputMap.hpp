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

#ifndef Overview_Components_Rocket_SDLInputMap_hpp
#define Overview_Components_Rocket_SDLInputMap_hpp

#include "cinek/allocator.hpp"
#include "cinek/map.hpp"

#include <Rocket/Core/Input.h>
#include <SDL2/SDL_keyboard.h>

namespace cinek {
    namespace overview {

    class RocketSDLInputMap
    {
    public:
        RocketSDLInputMap();

        Rocket::Core::Input::KeyIdentifier translateSDLKey(SDL_Keysym key) const;

        struct SDLKey
        {
            SDL_Keycode code;
            uint16_t mod;
        };

        SDLKey translateRocketKey(Rocket::Core::Input::KeyIdentifier key, uint16_t mod) const;

    private:
        unordered_map<SDL_Keycode, Rocket::Core::Input::KeyIdentifier> _keyMap;
        unordered_map<uint32_t, SDL_Keycode> _keyMapToSDL;
    };

    }   // namespace overview
}   // namespace cinek

#endif
