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
        _textBatch(),
        _bitmapBatch(),
        _batchState(),
        _currentFont(nullptr),
        _currentAtlas(),
        _currentAtlasId(kCinekBitmapAtlas_Invalid),
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
        //
        _textBatch = std::move(GLVertexBatch(GL_TRIANGLES, kBatchLimit*2, getAllocator()));
        _bitmapBatch = std::move(GLVertexBatch(GL_TRIANGLES, kBatchLimit*2, getAllocator()));

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
        
        _textBatch = std::move(GLVertexBatch());
        _bitmapBatch = std::move(GLVertexBatch());

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

        /**
         * @todo - we may not need _currentAtlas to be a shared_ptr if it's only valid
         * during the frame.
         */
        _currentAtlas = nullptr;
        _currentFont = nullptr;
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

    void GL3Renderer::drawRect(const Rect& rect, const Style& style)
    {
        drawRoundedRect(rect, {0,0,0,0}, style);
    }

    void GL3Renderer::drawRoundedRect(const Rect& rect, const std::array<int32_t, 4>& radii,
                                      const Style& style)
    {
      
    }

    void GL3Renderer::drawText(const char* text, int32_t x, int32_t y,
                               const Style& style)
    {
        GLVertexBatch* batch = obtainBatch(BatchState::makeText(style.textFont));
        if (!batch || !_currentFont)
            return;

        const GL3Texture& texture = (const GL3Texture&)_currentFont->getTexture();
        const RGBAColor& color = style.textColor;
        glm::vec4 colorF = glm::vec4(color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f);

        const char* curtext = text;
        const size_t kDrawLimit = 65536;
        //int32_t ox = x;
        do
        {
            int c = (unsigned char)*curtext;
            if (c == '\t')
            {
            //    for (int i = 0; i < 4; ++i)
            //    {
            //        if (x < g_tabStops[i]+ox)
            //        {
            //            x = g_tabStops[i]+ox;
            //            break;
            //        }
            //    }
            }
            else
            {
                if (batch->full())
                {
                    flushBatch();
                }
                if (!c)
        /* BREAK */ break;

                const stbtt_bakedchar& bakedChar = _currentFont->getChar(c);
                /* @todo Insert our uvs from a GLFont instead of using stbtt_bakedchar */
                batch->pushUV(bakedChar.x0/(float)texture.width(), bakedChar.y1/(float)texture.height());
                batch->pushUV(bakedChar.x0/(float)texture.width(), bakedChar.y0/(float)texture.height());
                batch->pushUV(bakedChar.x1/(float)texture.width(), bakedChar.y1/(float)texture.height());
                batch->pushUV(bakedChar.x1/(float)texture.width(), bakedChar.y1/(float)texture.height());
                batch->pushUV(bakedChar.x0/(float)texture.width(), bakedChar.y0/(float)texture.height());
                batch->pushUV(bakedChar.x1/(float)texture.width(), bakedChar.y0/(float)texture.height());
                batch->pushPos((float)(x+bakedChar.xoff), (float)(y+bakedChar.yoff+bakedChar.y1-bakedChar.y0));
                batch->pushPos((float)(x+bakedChar.xoff), (float)(y+bakedChar.yoff));
                batch->pushPos((float)(x+bakedChar.xoff+bakedChar.x1-bakedChar.x0),
                                        (float)(y+bakedChar.yoff+bakedChar.y1-bakedChar.y0));
                batch->pushPos((float)(x+bakedChar.xoff+bakedChar.x1-bakedChar.x0),
                                        (float)(y+bakedChar.yoff+bakedChar.y1-bakedChar.y0));
                batch->pushPos((float)(x+bakedChar.xoff), (float)(y+bakedChar.yoff));
                batch->pushPos((float)(x+bakedChar.xoff+bakedChar.x1-bakedChar.x0),
                                        (float)(y+bakedChar.yoff));
                batch->pushColor(colorF);
                batch->pushColor(colorF);
                batch->pushColor(colorF);
                batch->pushColor(colorF);
                batch->pushColor(colorF);
                batch->pushColor(colorF);
                x += bakedChar.xadvance;
            }
            ++curtext;
        }
        while ((curtext - text) < kDrawLimit);
        
        if (batch->full())
        {
            flushBatch();
        }
    }

    void GL3Renderer::setBitmapAtlas(cinek_bitmap_atlas atlas)
    {
        _currentAtlasId = atlas;
    }

    void GL3Renderer::drawBitmapFromAtlas(cinek_bitmap_index bitmapIndex, 
                                          int32_t x, int32_t y, float alpha)
    {
        GLVertexBatch* batch = obtainBatch(BatchState::makeBitmap(_currentAtlasId));
        if (!batch || !_currentAtlas)
            return;

        /** 
         * @todo Have a GLBitmapInfo that contains normalized UVs.
         */
        const glx::BitmapInfo* bitmapInfo = _currentAtlas->getBitmapFromIndex(bitmapIndex);
        if (!bitmapInfo)
        {
            RENDER_LOG_WARN("GL3Renderer.drawBitmapFromAtlas - bitmap %d not found in current atlas", bitmapIndex);
            return;
        }
        //  suboptimal - see todo.
        const GL3Texture& texture = static_cast<const GL3Texture&>(_currentAtlas->getTexture());
        
        batch->pushUV(bitmapInfo->x/(float)texture.width(), (bitmapInfo->y+bitmapInfo->h)/(float)texture.height());
        batch->pushUV(bitmapInfo->x/(float)texture.width(), bitmapInfo->y/(float)texture.height());
        batch->pushUV((bitmapInfo->x+bitmapInfo->w)/(float)texture.width(), bitmapInfo->y/(float)texture.height());
        batch->pushUV((bitmapInfo->x+bitmapInfo->w)/(float)texture.width(), bitmapInfo->y/(float)texture.height());
        batch->pushUV((bitmapInfo->x+bitmapInfo->w)/(float)texture.width(), (bitmapInfo->y+bitmapInfo->h)/(float)texture.height());
        batch->pushUV(bitmapInfo->x/(float)texture.width(), (bitmapInfo->y+bitmapInfo->h)/(float)texture.height());
        
        batch->pushPos((float)(x + bitmapInfo->offX), (float)(y - bitmapInfo->srcH + bitmapInfo->offY + bitmapInfo->h));
        batch->pushPos((float)(x + bitmapInfo->offX), (float)(y - bitmapInfo->srcH + bitmapInfo->offY));
        batch->pushPos((float)(x + bitmapInfo->offX + bitmapInfo->w), (float)(y - bitmapInfo->srcH + bitmapInfo->offY));
        batch->pushPos((float)(x + bitmapInfo->offX + bitmapInfo->w), (float)(y - bitmapInfo->srcH + bitmapInfo->offY));
        batch->pushPos((float)(x + bitmapInfo->offX + bitmapInfo->w), (float)(y - bitmapInfo->srcH + bitmapInfo->offY + bitmapInfo->h));
        batch->pushPos((float)(x + bitmapInfo->offX), (float)(y - bitmapInfo->srcH + bitmapInfo->offY + bitmapInfo->h));
        
        const glm::vec4 color = glm::vec4(1.0f,1.0f,1.0f,alpha);
        batch->pushColor(color);
        batch->pushColor(color);
        batch->pushColor(color);
        batch->pushColor(color);
        batch->pushColor(color);
        batch->pushColor(color);
        
        if (batch->full())
        {
            flushBatch();
        }
    }
    
    
    GLVertexBatch* GL3Renderer::obtainBatch(const BatchState& state)
    {
        //  determine if we need to render the current batch, i.e. any changes in state
        if (state == _batchState)
        {
            if (_batchState.mode == BatchState::kText && !_textBatch.full())
                return &_textBatch;
            else if (_batchState.mode == BatchState::kBitmap && !_bitmapBatch.full())
                return &_bitmapBatch;
        }
        
        flushBatch();
        
        _batchState = state;
        
        GLVertexBatch* batch = nullptr;

        switch( _batchState.mode )
        {
            case BatchState::kText:
                _currentFont = getFontLibrary().getFont(_batchState.fontHandle);
                if (_currentFont)
                {
                    batch = &_textBatch;
                }
                break;
            case BatchState::kBitmap:
                _currentAtlas = getBitmapLibrary().getAtlas(_batchState.atlasId);
                if (_currentAtlas)
                {
                    batch = &_bitmapBatch;
                }
                break;
            default:
                break;
        };
        
        if (!batch)
        {
            RENDER_LOG_WARN("GL3Renderer.setBatch - error setting batch state (%d)", _batchState.mode);
        }
        
        return batch;
    }
    
    void GL3Renderer::flushBatch()
    {
        GLVertexBatch* batch = nullptr;
        const GL3Texture* texture = nullptr;
        GLuint program = 0;

        switch (_batchState.mode)
        {
            case BatchState::kBitmap:
                if (_currentAtlas)
                {
                    texture = (const GL3Texture *)&_currentAtlas->getTexture();
                }
                program = _standardShader;
                batch = &_bitmapBatch;
                break;
            case BatchState::kText:
                if (_currentFont)
                {
                    texture = (const GL3Texture *)&_currentFont->getTexture();
                }
                program = _textShader;
                batch = &_textBatch;
                break;
            default:
                return;
        };
        
        if (texture)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture->getTextureId());
        }
       
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
            
        batch->draw();
    }
        
    }   // namespace glx
}   // namespace cinekine
