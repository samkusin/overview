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
#include "GL3Texture.hpp"
#include "GLShaderLibrary.hpp"
#include "GLVertexBatch.hpp"
 
#include "SDL2/SDL_video.h"

#include "glm/gtc/matrix_transform.hpp"

#include <vector>

namespace cinekine {
    namespace glx {

    class Texture;
    
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

        virtual void begin();
        virtual void display();
        virtual Rect getViewport() const;
        virtual void setViewport(const Rect& rect);
        
        virtual void clear(const RGBAColor& color);
        virtual void drawTextureRect(const Texture& texture,
                                     const Rect& source, const Rect& dest,
                                     const RGBAColor& color);

    private:
        SDL_Window* _window;
        SDL_GLContext _glContext;
        GLShaderLibrary _shaderLibrary;
        GLuint _standardShader;
        GLuint _textShader;

        // Vertex batches used for batched rendering
        vector<GLVertexBatch> _batch;
        uint32_t _batchIndex;
        
        struct BatchState
        {
            GLuint textureId;
            GL3Texture::SamplerFormat textureSamplerFormat;
            BatchState(): textureId(0), textureSamplerFormat(GL3Texture::kFormatNone) {}
            BatchState(const GL3Texture& texture):
                textureId(texture.textureID()),
                textureSamplerFormat(texture.samplerFormat()) {}
           
            bool operator==(const BatchState& s) const;
            void clear() { textureId = 0; }
        };
        BatchState _batchState;

        glm::mat4 _projectionMat;
        
    private:
        GLVertexBatch& obtainBatch(const BatchState& state);
        GLVertexBatch& flushBatch();
    };
    
    inline bool GL3Renderer::BatchState::operator==(const GL3Renderer::BatchState& s) const
    {
        return textureId == s.textureId;
    }

    }   // namespace glx
}   // namespace cinekine

#endif
