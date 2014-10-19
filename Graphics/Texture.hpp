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
 * @file    Texture.hpp
 * @author  Samir Sinha
 * @date    09/10/2013
 * @brief   A cross-platform Texture interface
 * @copyright Cinekine
 */

#ifndef CK_Graphics_Texture_hpp
#define CK_Graphics_Texture_hpp

#include "Graphics/Rect.hpp"
#include "cinek/rendertypes.h"

#include <memory>

namespace cinek {
    namespace glx {

    /**
     * @class Texture
     * @brief A cross-platform texture resource used by Renderer implementations.
     */
    class Texture
    {
    public:
        virtual ~Texture() {}
        /** @return Checks whether texture exists or was created successfully */
        virtual operator bool() const = 0;

        /** @return The texture width in pixels */
        virtual uint32_t width() const = 0;
        /** @return The texture height in pixels */
        virtual uint32_t height() const = 0;
    };

    /** A Texture managed pointer */
    typedef std::shared_ptr<Texture> TexturePtr;

    }   // namespace glx
}   // namespace cinek

#endif
