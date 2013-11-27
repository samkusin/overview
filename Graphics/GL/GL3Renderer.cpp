//
//  GL/GL3Renderer.cpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "GL3Renderer.hpp"
#include "GL3Texture.hpp"
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
        _batch(std_allocator<GLVertexBatch>(allocator)),
        _batchIndex(0),
        _batchState(),
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
        
        glUseProgram(_standardShader);

        // Default GL State Setup
        const size_t kBatchLimit = 16;
        const size_t kBatchPrimLimit = 2048;
        
        _batch.reserve(kBatchLimit);
        while (_batch.size() != _batch.capacity())
        {
            _batch.emplace_back(GL_TRIANGLES, kBatchPrimLimit, getAllocator());
        }

        //  orthographic projection 
        int w, h;
        SDL_GetWindowSize(_window, &w, &h);
        _projectionMat = glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f );

        //  alpha blending enabled (standard 1-src blending)
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    GL3Renderer::~GL3Renderer()
    {
        _shaderLibrary.unloadProgram(_textShader);
        _shaderLibrary.unloadProgram(_standardShader);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        _batch.clear();

        if (_glContext)
        {
            SDL_GL_DeleteContext(_glContext);
            _glContext = NULL;
        }
    }

    unique_ptr<Texture> GL3Renderer::loadTexture(const char* pathname)
    {
        // TODO - perhaps we need a make_unique_ptr (C++11 version) to confirm that the allocator
        //  creating the item is also used to delete the item.
        unique_ptr<Texture> texture(getAllocator().newItem<GL3Texture>(*this, pathname), getAllocator());
        if (!(*texture.get()))
        {
            texture.release();
        }
        return std::move(texture);
    }

    unique_ptr<Texture> GL3Renderer::createTextureFromBuffer(uint16_t w, uint16_t h,
            cinek_pixel_format format,
            const uint8_t* bytes)
    {
        unique_ptr<Texture> texture(getAllocator().newItem<GL3Texture>(*this, w, h, format, bytes), getAllocator());
        if (!(*texture.get()))
        {
            texture.release();
        }
        return std::move(texture);
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
        GLint data[4];
        glGetIntegerv(GL_VIEWPORT, data);
        return Rect::rectFromDimensions(data[0], data[1], data[2], data[3]);
    }

    void GL3Renderer::setViewport(const Rect& rect)
    {
        glViewport(rect.left, rect.top, rect.width(), rect.height());
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
        GLVertexBatch& batch = obtainBatch(BatchState(gltexture));
          
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
        
        if (batch.full())
        {
            flushBatch();
        }
    }
    
    GLVertexBatch& GL3Renderer::obtainBatch(const BatchState& state)
    {
        //  determine if we need to render the current batch, i.e. any changes in state
        GLVertexBatch& batch = _batch[_batchIndex % _batch.capacity()];
        if (state == _batchState && !batch.full())
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
        
        if (texture)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
        }
        if (program)
        {
            glUseProgram(program);
       
            GLint projectionMatUniform = glGetUniformLocation(program, kGL_ShaderUniformProjectionMat);
            if (projectionMatUniform < 0)
            {
                // required
                RENDER_LOG_WARN("GL3Renderer.flushBatch - selected shader does not have a uniform: %s",
                                kGL_ShaderUniformProjectionMat);
            }
            GLint texSamplerUniform = glGetUniformLocation(program, kGL_ShaderUniformTextureSampler0);
            if (texSamplerUniform < 0)
            {
                RENDER_LOG_WARN("GL3Renderer.flushBatch - selected shader does not have a uniform: %s",
                                kGL_ShaderUniformTextureSampler0);
            }
            
            glUniformMatrix4fv(projectionMatUniform, 1, GL_FALSE, glm::value_ptr(_projectionMat));
            glUniform1i(texSamplerUniform, 0);
        }
            
        batch.draw();

        ++_batchIndex;
        return _batch[_batchIndex % _batch.capacity()];
    }
        
    }   // namespace glx
}   // namespace cinekine
