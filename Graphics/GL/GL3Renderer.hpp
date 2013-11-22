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
 * @file    GL/GL3Renderer.hpp
 * @author  Samir Sinha
 * @date    11/16/2013
 * @brief   A OpenGL3 high-level Renderer implementation
 * @copyright Cinekine
 */

#ifndef CK_Graphics_GL_GL3Renderer_hpp
#define CK_Graphics_GL_GL3Renderer_hpp

#include "Graphics/Renderer.hpp"
#include "GLShaderLibrary.hpp"

#include "SDL2/SDL_video.h"

#include "glm/gtc/matrix_transform.hpp"

#include <vector>

namespace cinekine {
    namespace glx {

    //  The OpenGL 3.x Renderer
    //
    class GL3Renderer: public Renderer
    {
    public:    
        GL3Renderer(const RendererInitParameters& initParams,
                 SDL_Window* window,
                 const Allocator& allocator);
        virtual ~GL3Renderer();
        
        virtual operator bool() const {
            return _glContext != NULL;
        }
        
        ///////////////////////////////////////////////////////////////////////
        // Resource Management
        //
        virtual unique_ptr<Texture> loadTexture(const char* pathname);

        virtual unique_ptr<Texture> createTextureFromBuffer(uint16_t w, uint16_t h,
            cinek_pixel_format format,
            const uint8_t* bytes);

        ///////////////////////////////////////////////////////////////////////
        //  Housekeeping methods, used to mark the beginning and end of a render
        //  frame.
        //  
        virtual void begin();
        virtual void display();
        virtual Rect getViewport() const;
        virtual void setViewport(const Rect& rect);
        
        ///////////////////////////////////////////////////////////////////////
        //  Renderer Drawing Methods
        //  All rendering methods act on the current target.
        //  
        virtual void clear(const RGBAColor& color);
        virtual void drawRect(const Rect& rect, const Style& style);
        virtual void drawRoundedRect(const Rect& rect, const std::array<int32_t, 4>& radii,
                                     const Style& style);
        virtual void drawText(const char* text, int32_t x, int32_t y,
                              const Style& style);
        virtual void setBitmapAtlas(cinek_bitmap_atlas atlas);

        virtual void drawBitmapFromAtlas(cinek_bitmap_index bitmapIndex, 
                                         int32_t x, int32_t y, float alpha);

    private:
        SDL_Window* _window;
        SDL_GLContext _glContext;
        GLShaderLibrary _shaderLibrary;
        GLuint _standardShader;
        GLint  _projectionMatUniform;
        GLint  _texSamplerUniform;
        GLint  _vertexColorOn;
        GLint  _colorBlendMode;

        static const size_t kTriQuadVertexCount = 6;
        static const size_t kQuadVertexCount = 4;
        static const size_t kTextCacheCharCount = 80;
        std::vector<glm::vec2, std_allocator<glm::vec2>> _vertCache;
        std::vector<glm::vec2, std_allocator<glm::vec2>> _uvCache;
        std::vector<glm::vec4, std_allocator<glm::vec4>> _colorCache;

        GLuint  _bitmapVAO;
        GLuint  _bitmapVBO[2];
        GLuint  _textVAO;
        GLuint  _textVBO[3];

        std::shared_ptr<BitmapAtlas> _currentAtlas;
        glm::mat4 _projectionMat;
        bool _setAtlasGLTexture;
    };
    

    }   // namespace glx
}   // namespace cinekine

#endif
