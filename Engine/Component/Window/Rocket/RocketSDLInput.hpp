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

#ifndef Overview_Components_Rocket_SDLInput_hpp
#define Overview_Components_Rocket_SDLInput_hpp


#include "cinek/cpp/ckalloc.hpp"
#include "cinek/cpp/ckmap.hpp"

#include "Rocket/Core/Input.h"
#include "SDL2/SDL_events.h"

#include <unordered_map>

namespace Rocket {
    namespace Core {
        class Context;
    }
}

namespace cinekine {
    namespace ovengine {

    class RocketSDLInput
    {
    public:
        RocketSDLInput();

        bool dispatchSDLEvent(const SDL_Event& event, Rocket::Core::Context* context);
    
    private:
        Rocket::Core::Input::KeyIdentifier translateSDLKey(SDL_Keysym key);

        unordered_map<SDL_Keycode, Rocket::Core::Input::KeyIdentifier> _keyMap;
    };

    }   // namespace ovengine
}   // namespace cinekine

#endif
