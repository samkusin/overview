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
 * @file    GL/GL3Texture.hpp
 * @author  Samir Sinha
 * @date    11/16/2013
 * @brief   A GL3 compatible texture object
 * @copyright Cinekine
 */

#ifndef CK_Graphics_GL_GL3_Texture_hpp
#define CK_Graphics_GL_GL3_Texture_hpp

#include "Graphics/Texture.hpp"
#include "GLUtils.hpp"

namespace cinekine {
    namespace glx {
    
    class Renderer;
    
    class GL3Texture: public Texture
    {
        CK_CLASS_NON_COPYABLE(GL3Texture);

    public:
        GL3Texture(Renderer& renderer, const char *pathname);
        GL3Texture(Renderer& renderer, uint16_t w, uint16_t h,
            cinek_pixel_format format,
            const uint8_t* bytes);
        GL3Texture(GL3Texture&& other);
        virtual ~GL3Texture();

        virtual operator bool() const {
            return _texture!=0;
        }
        GLuint textureID() const {
            return _texture;
        }
        virtual uint32_t width() const {
            return _width;
        }
        virtual uint32_t height() const {
            return _height;
        }

        //  used for selecting the shader to use for rendering ops
        enum SamplerFormat
        {
            kFormatNone,
            kFormatRGBA,
            kFormatRed
        };
        SamplerFormat samplerFormat() const {
            return _samplerFormat;
        }

    private:
        GLuint createTexture(uint32_t w, uint32_t h,
                             cinek_pixel_format format,
                             const uint8_t* bytes);
        Renderer& _renderer;
        GLuint _texture;
        uint32_t _width, _height;
        SamplerFormat _samplerFormat;
    };
        
    }   // namespace glx
}   // namespace cinekine

#endif