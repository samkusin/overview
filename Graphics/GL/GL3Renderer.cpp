//
//  GL/GL3Renderer.cpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "GL3Renderer.hpp"
#include "GL3Texture.hpp"
#include "GL3Mesh.hpp"
#include "GLShaderAttributes.hpp"

#include "Graphics/RenderDebug.hpp"

#include <array>

namespace cinekine {
    namespace glx {

    GL3Renderer::GL3Renderer(const RendererInitParameters& initParams,
                             SDL_Window* window,
                             const Allocator& allocator) :
        Renderer(initParams, allocator),
        _window(window),
        _glContext(NULL),
        _shaderLibrary("static/shaders/gl3", allocator),
        _standardShader(0),
        _textShader(0),
        _currentShader(0),
        _projectionMatUniform(0),
        _texSamplerUniform(0),
        _positionUniform(0),
        _vertsPos(std_allocator<glm::vec2>(allocator)),
        _vertsUV(std_allocator<glm::vec2>(allocator)),
        _vertsColor(std_allocator<glm::vec4>(allocator)),
        _batch(std_allocator<GLVertexBatch>(allocator)),
        _batchIndex(0),
        _batchState(),
        _meshVBOs {},
        _projectionMat()
    {
        _glContext = SDL_GL_CreateContext(_window);
        if (!_glContext)
        {
            RENDER_LOG_ERROR("GL3Renderer: failed to obtain a GL context: %s", SDL_GetError());
            return;
        }

        SDL_GL_SetSwapInterval(1);

        // load default shaders
        _standardShader = _shaderLibrary.loadProgram("standard.json");
        _textShader = _shaderLibrary.loadProgram("text.json");
        if (!_standardShader || !_textShader)
        {
            SDL_GL_DeleteContext(_glContext);
            _glContext = NULL;
            return;
        }

        // Default GL State Setup
        //  orthographic projection 
        int w, h;
        SDL_GetWindowSize(_window, &w, &h);
        _projectionMat = glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f );
        
        selectShader(_standardShader, glm::vec2());

        const size_t kBatchLimit = 16;
        const size_t kBatchTriLimit = 2048;

        _vertsPos.reserve(kBatchTriLimit*3);
        _vertsUV.reserve(kBatchTriLimit*3);
        _vertsColor.reserve(kBatchTriLimit*3);
        _batch.reserve(kBatchLimit);

        while (_batch.size() != _batch.capacity())
        {
            _batch.emplace_back(GL_STREAM_DRAW, _vertsPos, _vertsUV, _vertsColor);
        }

        glGenBuffers(4, _meshVBOs);

        //  alpha blending enabled (standard 1-src blending)
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    GL3Renderer::~GL3Renderer()
    {
        _shaderLibrary.unloadProgram(_textShader);
        _shaderLibrary.unloadProgram(_standardShader);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glDeleteBuffers(4, _meshVBOs);

        _batch.clear();

        if (_glContext)
        {
            SDL_GL_DeleteContext(_glContext);
            _glContext = NULL;
        }
    }

    TexturePtr GL3Renderer::loadTexture(const char* pathname)
    {
        Allocator& allocator = getAllocator();
        std::shared_ptr<Texture> texture = std::allocate_shared<GL3Texture,
                                                                std_allocator<GL3Texture>, 
                                                                GL3Renderer&, const char*>
            (
                std_allocator<GL3Texture>(allocator), *this, std::move(pathname)
            );

        return texture;
    }

    TexturePtr GL3Renderer::createTextureFromBuffer(uint32_t w, uint32_t h,
            cinek_pixel_format format,
            const uint8_t* bytes)
    {
        Allocator& allocator = getAllocator();
        std::shared_ptr<Texture> texture = std::allocate_shared<GL3Texture,
                                                                std_allocator<GL3Texture>, 
                                                                GL3Renderer&,
                                                                uint32_t, uint32_t,
                                                                cinek_pixel_format,
                                                                const uint8_t*>
            (
                std_allocator<GL3Texture>(allocator), *this,
                                          std::move(w), std::move(h),
                                          std::move(format), std::move(bytes)
            );

        return texture;
    }
    
    void GL3Renderer::begin()
    {
        SDL_GL_MakeCurrent(_window, _glContext);
    }
        
    void GL3Renderer::display()
    {
        flushBatch();

        _batchState.clear();
        
        SDL_GL_SwapWindow(_window);
    }

    Rect GL3Renderer::getViewport() const
    {
        int w, h;
        SDL_GetWindowSize(_window, &w, &h);
        GLint data[4];
        glGetIntegerv(GL_VIEWPORT, data);
        return Rect::rectFromDimensions(data[0], h - data[1], data[2], data[3]);
    }

    void GL3Renderer::setViewport(const Rect& rect)
    {
        int w, h;
        SDL_GetWindowSize(_window, &w, &h);
        glViewport(rect.left, h - rect.bottom, rect.width(), rect.height());
    }

    void GL3Renderer::enableScissor()
    {
        glEnable(GL_SCISSOR_TEST);
    }

    void GL3Renderer::disableScissor()
    {
        glDisable(GL_SCISSOR_TEST);
    }

    void GL3Renderer::setScissor(const Rect& rect)
    {
        if (!glIsEnabled(GL_SCISSOR_TEST))
        {
            glEnable(GL_SCISSOR_TEST);
        }
        int w, h;
        SDL_GetWindowSize(_window, &w, &h);
        glScissor(rect.left, h - rect.bottom, rect.width(), rect.height());
    }

    void GL3Renderer::clear(const RGBAColor& color)
    {
        glClearColor(color.r/255.0f,color.g/255.0f,color.b/255.0f,color.a/255.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void GL3Renderer::drawTextureRect(const Texture& texture,
                                      const Rect& source, const Rect& dest,
                                      const RGBAColor& color)
    {
        const GL3Texture& gltexture = static_cast<const GL3Texture&>(texture);
        GLVertexBatch& batch = obtainBatch(BatchState(Mesh::kTriangles, gltexture), 6);
          
        float tWscale = 1.0f/(float)texture.width();
        float tHscale = 1.0f/(float)texture.height();

        batch.pushUV(source.left*tWscale, source.bottom*tHscale);
        batch.pushUV(source.left*tWscale, source.top*tHscale);
        batch.pushUV(source.right*tWscale, source.top*tHscale);
        batch.pushUV(source.right*tWscale, source.top*tHscale);
        batch.pushUV(source.right*tWscale, source.bottom*tHscale);
        batch.pushUV(source.left*tWscale, source.bottom*tHscale);
        
        batch.pushPos((float)dest.left, (float)dest.bottom);
        batch.pushPos((float)dest.left, (float)dest.top);
        batch.pushPos((float)dest.right, (float)dest.top);
        batch.pushPos((float)dest.right, (float)dest.top);
        batch.pushPos((float)dest.right, (float)dest.bottom);
        batch.pushPos((float)dest.left, (float)dest.bottom);

        const glm::vec4 colorF = glm::vec4(color.r/255.0f,
                                           color.g/255.0f,
                                           color.b/255.0f,
                                           color.a/255.0f);
        batch.pushColor(colorF);
        batch.pushColor(colorF);
        batch.pushColor(colorF);
        batch.pushColor(colorF);
        batch.pushColor(colorF);
        batch.pushColor(colorF);
    }

    void GL3Renderer::drawMeshVertices(const Texture& texture, Mesh::Type meshType,
                                       const cinekine::vector<glm::vec2>& vertsPos,
                                       const cinekine::vector<glm::vec2>& vertsUV,
                                       const cinekine::vector<glm::vec4>& vertsColor,
                                       const cinekine::vector<GLushort>& indices)
    {
        //  just push the vertices up - this code will orphan buffers (depending on driver impl)
        //  this should be a call of last resort.  the rule is mesh first, then streaming tris before
        //  deciding to use this method
        //
        GLenum drawMode;
        if (meshType == Mesh::kTriangles)
            drawMode = GL_TRIANGLES;
        else if (meshType == Mesh::kTriangleFan)
            drawMode = GL_TRIANGLE_FAN;
        else
            return;

        const GL3Texture& glTexture = static_cast<const GL3Texture&>(texture);
        GLuint textureId = 0;
        if (glTexture)
        {
            textureId = glTexture.textureID();
        }
        else
        {
            //  use backup stock texture for flat shading?
            CK_ASSERT(false);
            return;
        }
        
        //  draw plain vertices or a true mesh (with vertex indices)
        glBindBuffer(GL_ARRAY_BUFFER, _meshVBOs[0]);
        glBufferData(GL_ARRAY_BUFFER, vertsPos.size() * sizeof(glm::vec2), vertsPos.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrPos);
        glVertexAttribPointer(kGL_ShaderVertexAttrPos, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, _meshVBOs[1]);
        glBufferData(GL_ARRAY_BUFFER, vertsUV.size() * sizeof(glm::vec2), vertsUV.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrUVs);
        glVertexAttribPointer(kGL_ShaderVertexAttrUVs, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glBindBuffer(GL_ARRAY_BUFFER, _meshVBOs[2]);
        glBufferData(GL_ARRAY_BUFFER, vertsColor.size() * sizeof(glm::vec4), vertsColor.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrColor);
        glVertexAttribPointer(kGL_ShaderVertexAttrColor, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        if (!indices.empty())
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshVBOs[3]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);
        }
        
        selectShader(this->_standardShader, glm::vec2());

        if (textureId)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureId);
        }
        if (indices.empty())
        {
            glDrawArrays(drawMode, 0, vertsPos.size());
        }
        else
        {
            glDrawElements(drawMode, indices.size(), GL_UNSIGNED_SHORT, NULL);
        }
    }

    MeshPtr GL3Renderer::createMesh(TexturePtr& texture,
                                    Mesh::Type meshType,
                                    const cinekine::vector<glm::vec2>& vertsPos,
                                    const cinekine::vector<glm::vec2>& vertsUV,
                                    const cinekine::vector<glm::vec4>& vertsColor,
                                    const cinekine::vector<uint16_t>& indices)
    {
        Allocator& allocator = getAllocator();
        std::shared_ptr<Mesh> mesh = std::allocate_shared<GL3Mesh,
                                                          std_allocator<GL3Mesh>, 
                                                          TexturePtr&,
                                                          Mesh::Type,
                                                          const vector<glm::vec2>&,
                                                          const vector<glm::vec2>&,
                                                          const vector<glm::vec4>&,
                                                          const vector<uint16_t>& >
            (
                std_allocator<GL3Mesh>(allocator),
                texture,
                std::move(meshType),
                vertsPos, vertsUV, vertsColor, indices
            );

        return mesh;    
    }

    void GL3Renderer::drawMesh(const Mesh& mesh, const glm::vec2& position)
    {
        flushBatch();

        selectShader(_standardShader, position);

        const GL3Mesh& glMesh = static_cast<const GL3Mesh&>(mesh);
        glMesh.draw();
    }
    
    GLVertexBatch& GL3Renderer::obtainBatch(const BatchState& state, size_t vertexRequest)
    {
        //  determine if we need to render the current batch, i.e. any changes in state
        GLVertexBatch& batch = _batch[_batchIndex % _batch.capacity()];
        if (_batchState.drawMode == Mesh::kUndefined)
        {
            // start of frame
            _batchState = state;
            return batch;
        }
    
        if (state == _batchState && batch.available() >= vertexRequest)
        {
            return batch; 
        }
        
        GLVertexBatch& nextbatch = flushBatch();
        
        _batchState = state;
        return nextbatch;
    }
    
    GLVertexBatch& GL3Renderer::flushBatch()
    {
        GLVertexBatch& batch = _batch[_batchIndex % _batch.capacity()];
        if (_batch.empty())
            return batch;
        
        GLuint texture = _batchState.textureId;
        GLuint program = 0;

        //  select program based on texture format
        switch (_batchState.textureSamplerFormat)
        {
            case GL3Texture::kFormatRGBA:
                program = _standardShader;
                break;
            case GL3Texture::kFormatRed:
                program = _textShader;
                break;
            default:
                break;
        }
        
        GLenum drawMode = GL_POINTS;
        if (_batchState.drawMode == Mesh::kTriangles)
        {
            drawMode = GL_TRIANGLES;
        }
        else if (_batchState.drawMode == Mesh::kTriangleFan)
        {
            drawMode = GL_TRIANGLE_FAN;
        }
        else
        {
            RENDER_LOG_WARN("GL3Renderer.flushBatch Undefined draw mode (%d) specified", _batchState.drawMode);
        }
        if (texture)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
        }
        if (program)
        {
           selectShader(program, glm::vec2());
        }
        
        batch.draw(drawMode);
        
        //  wipe the scratchpad vertex vectors used to upload data into the current GLVertexBatch
        _vertsPos.clear();
        _vertsUV.clear();
        _vertsColor.clear();

        ++_batchIndex;
        return _batch[_batchIndex % _batch.capacity()];
    }
        
    void GL3Renderer::selectShader(GLuint shader, const glm::vec2& position)
    {
        if (shader != _currentShader)
        {
            glUseProgram(shader);
       
            _projectionMatUniform = glGetUniformLocation(shader, kGL_ShaderUniformProjectionMat);
            if (_projectionMatUniform < 0)
            {
                // required
                RENDER_LOG_WARN("GL3Renderer.selectShader - selected shader does not have a uniform: %s",
                                kGL_ShaderUniformProjectionMat);
            }
            _texSamplerUniform = glGetUniformLocation(shader, kGL_ShaderUniformTextureSampler0);
            if (_texSamplerUniform < 0)
            {
                RENDER_LOG_WARN("GL3Renderer.selectShader - selected shader does not have a uniform: %s",
                                kGL_ShaderUniformTextureSampler0);
            }
            _positionUniform = glGetUniformLocation(shader, kGL_ShaderUniformPosition);
            if (_positionUniform < 0)
            {
                // required
                RENDER_LOG_WARN("GL3Renderer.selectShader - selected shader does not have a uniform: %s",
                                kGL_ShaderUniformPosition);        
            }
        }
        glUniformMatrix4fv(_projectionMatUniform, 1, GL_FALSE, glm::value_ptr(_projectionMat));
        glUniform1i(_texSamplerUniform, 0);
        glUniform2f(_positionUniform, position.x, position.y);
    }

    }   // namespace glx
}   // namespace cinekine
