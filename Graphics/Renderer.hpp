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
 * @file    Renderer.hpp
 * @author  Samir Sinha
 * @date    11/26/2013
 * @brief   The Renderer Interface as used by the Engine
 * @copyright Cinekine
 */

#ifndef CK_Graphics_Renderer_hpp
#define CK_Graphics_Renderer_hpp

#include "RendererCLI.hpp"

#include "SDL2/SDL_video.h"

namespace cinekine {
    namespace glx {
    
    class Theater;

    //  Extra parameters for initializing the renderer.
    struct RendererInitParameters
    {
        const char* bitmapAtlasDir;
    };

    /**
     * @class Renderer
     * @brief The base interface for all platform-specific rendering
     *        implementations.
     */
    class Renderer : public RendererCLI
    {
    public:    
        Renderer(const RendererInitParameters& initParams,
                 const Allocator& allocator);
        virtual ~Renderer() = default;
       
        /**
         * Used to verify that the renderer initialized
         * @return True if the renderer initialized, or False on error
         */
        virtual operator bool() const = 0;

        /**
         *  Call at the start of rendering a frame
         */
        virtual void begin() = 0;
        /**
         *  Display the rendered frame
         */
        virtual void display() = 0;
        /**
         * Clears the entire client region (entire screen if fullscreen)
         * @param color A RGBA color value.
         */
        virtual void clear(const RGBAColor& color) = 0;
        /**
         * @return The Renderer's allocator interface
         */
        const Allocator& getAllocator() const {
            return _allocator;
        }
        /**
         * @return The Renderer's allocator interface
         */
        Allocator& getAllocator() {
            return _allocator;
        }

    private:
        Allocator _allocator;
    };
    

    }   // namespace glx
}   // namespace cinekine

#endif
