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

#include "cinek/cpp/ckvector.hpp"

#include <array>

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
        virtual TexturePtr loadTexture(const char* pathname);

        virtual TexturePtr createTextureFromBuffer(uint32_t w, uint32_t h,
                                                   cinek_pixel_format format,
                                                   const uint8_t* bytes);

        virtual void begin();
        virtual void display();
        virtual Rect getViewport() const;
        virtual void setViewport(const Rect& rect);
        virtual void enableScissor();
        virtual void disableScissor();
        virtual void setScissor(const Rect& rect);
        
        virtual void clear(const RGBAColor& color);
        virtual void drawTextureRect(const Texture& texture,
                                     const Rect& source, const Rect& dest,
                                     const RGBAColor& color);

        virtual void drawMeshVertices(const Texture& texture, Mesh::Type meshType,
                                      const cinekine::vector<glm::vec2>& vertsPos,
                                      const cinekine::vector<glm::vec2>& vertsUV,
                                      const cinekine::vector<glm::vec4>& vertsColor,
                                      const cinekine::vector<GLushort>& indices);

        virtual MeshPtr createMesh(TexturePtr& texture,
                                   Mesh::Type meshType,
                                   const cinekine::vector<glm::vec2>& vertsPos,
                                   const cinekine::vector<glm::vec2>& vertsUV,
                                   const cinekine::vector<glm::vec4>& vertsColor,
                                   const cinekine::vector<ushort>& indices);

        virtual void drawMesh(const Mesh& mesh, const glm::vec2& position);


    private:
        SDL_Window* _window;
        SDL_GLContext _glContext;
        GLShaderLibrary _shaderLibrary;
        GLuint _standardShader;
        GLuint _textShader;
        GLuint _currentShader;
        GLint _projectionMatUniform;
        GLint _texSamplerUniform;
        GLint _positionUniform;

        // Vertex batches used for batched rendering
        vector<glm::vec2> _vertsPos;
        vector<glm::vec2> _vertsUV;
        vector<glm::vec4> _vertsColor;
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

        GLuint _meshVBOs[4];

        glm::mat4 _projectionMat;
        
    private:
        GLVertexBatch& obtainBatch(const BatchState& state);
        GLVertexBatch& flushBatch();
        void selectShader(GLuint shader, const glm::vec2& position);
    };
    
    inline bool GL3Renderer::BatchState::operator==(const GL3Renderer::BatchState& s) const
    {
        return textureId == s.textureId;
    }

    }   // namespace glx
}   // namespace cinekine

#endif
