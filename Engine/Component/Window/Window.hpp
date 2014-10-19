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
 *
 * @file    Window.hpp
 * @author  Samir Sinha
 * @date    12/4/2013
 * @brief   Base interface for all UIWindow implementations
 * @copyright Cinekine
 */

#ifndef Overview_Components_Window_hpp
#define Overview_Components_Window_hpp

#include "cinek/allocator.hpp"

#include <SDL2/SDL_keycode.h>

#include <functional>

namespace cinek {
    namespace overview {

    /**
     * @class Window
     * @brief Interface for all Window implementations
     */
    class Window
    {
    public:
        virtual ~Window() {}

        /**
         * Shows the window
         */
        virtual void show() = 0;
    };

    typedef std::shared_ptr<Window> WindowPtr;

    }   // namespace overview
}   // namespace cinek

#endif