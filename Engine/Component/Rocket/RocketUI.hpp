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

#ifndef Overview_Components_Rocket_UIComponent_hpp
#define Overview_Components_Rocket_UIComponent_hpp

#include "../UIComponentImpl.hpp"
#include "RocketSDLInput.hpp"
#include "RocketFileInterface.hpp"
#include "RocketSystemInterface.hpp"
#include "RocketRenderInterface.hpp"

#include "Rocket/Core.h"

namespace cinekine {
    namespace glx {
        class Renderer;
    }

    namespace ovengine {

    class RocketUI: public UIComponent::Impl
    {
    public:
        RocketUI(glx::Renderer& renderer, Allocator& allocator);
        ~RocketUI();

        operator bool() const;

        void update(cinek_time currentTime);
        void render();

        void handleInput(const SDL_Event& event);

        /**
         * Loads a UI window/controller identified by the named resource
         * @param  name Resource name
         * @return      Created UIWindow pointer
         */
        UIWindow* createWindow(const char* name);

    private:
        Allocator _allocator;
        RocketSystemInterface _rocketSystem;
        RocketFileInterface _rocketFile;
        RocketRenderInterface _rocketRenderer;
        bool _coreInitialized;

        RocketSDLInput _sdlInput;
        Rocket::Core::Context *_context;
    };

    }   // namespace ovengine
}   // namespace cinekine

#endif
