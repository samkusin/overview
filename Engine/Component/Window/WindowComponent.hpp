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

#ifndef Overview_Components_WindowComponent_hpp
#define Overview_Components_WindowComponent_hpp

#include "WindowComponentClient.hpp"

#include "cinek/cpp/ckalloc.hpp"

#include "SDL2/SDL_events.h"

namespace cinekine {
    namespace glx {
        class Renderer;
    }
}

namespace cinekine {
    namespace ovengine {

    class WindowComponent: public WindowComponentClient
    {
    public:
        virtual ~WindowComponent() = default;

        virtual operator bool() const = 0;

        virtual void update(cinek_time currentTime) = 0;
        virtual void render() = 0;

        virtual void handleInput(const SDL_Event& event) = 0;
    };

    /** Handle to a WindowComponent */
    typedef unique_ptr<WindowComponent> WindowComponentPtr;

    /**
     * Creates a Window Component based on the specified identifier
     * @param  name      Component name
     * @param  renderer  Renderer interface
     * @param  allocator Memory allocator used by the component
     * @return           Shared reference to a WindowComponent
     */
    WindowComponentPtr createWindowComponent(const char* name,
                                             glx::Renderer& renderer,
                                             const Allocator& allocator);


    }   // namespace ovengine
}   // namespace cinekine

#endif
