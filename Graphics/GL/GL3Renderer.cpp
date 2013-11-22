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
        _projectionMatUniform(-1),
        _texSamplerUniform(-1),
        _vertexColorOn(0),
        _colorBlendMode(0),
        _vertCache(std_allocator<glm::vec2>(allocator)),
        _uvCache(std_allocator<glm::vec2>(allocator)),
        _colorCache(std_allocator<glm::vec4>(allocator)),
        _bitmapVAO {0},
        _bitmapVBO {0,0},
        _textVAO {0},
        _textVBO {0,0},
        _currentAtlas(),
        _projectionMat(),
        _setAtlasGLTexture(false)
    {
        _glContext = SDL_GL_CreateContext(_window);
        if (!_glContext)
        {
            RENDER_LOG_ERROR("GL3Renderer: failed to obtain a GL context: %s", SDL_GetError());
            return;
        }

        // load default shaders
        _standardShader = _shaderLibrary.loadProgram("standard.json");
        if (!_standardShader)
        {
            SDL_GL_DeleteContext(_glContext);
            _glContext = NULL;
            return;
        }

        // Default GL State Setup
        // 
        glUseProgram(_standardShader);
        _projectionMatUniform = glGetUniformLocation(_standardShader, kGL_ShaderUniformProjectionMat);
        if (_projectionMatUniform < 0)
        {
            // required
            RENDER_LOG_WARN("GL3Renderer - selected shader does not have a uniform: %s",
                            kGL_ShaderUniformProjectionMat);
        }
        _texSamplerUniform = glGetUniformLocation(_standardShader, kGL_ShaderUniformTextureSampler0);
        if (_texSamplerUniform < 0)
        {
            RENDER_LOG_WARN("GL3Renderer - selected shader does not have a uniform: %s",
                            kGL_ShaderUniformTextureSampler0);
        }
        _vertexColorOn = glGetUniformLocation(_standardShader, kGL_ShaderUniformVertexColorEnabled);
        if (_vertexColorOn < 0)
        {
            // required
            RENDER_LOG_WARN("GL3Renderer - selected shader does not have a uniform: %s",
                            kGL_ShaderUniformVertexColorEnabled);
        }
        _colorBlendMode = glGetUniformLocation(_standardShader, kGL_ShaderUniformColorBlendMode);
        if (_colorBlendMode < 0)
        {
            // required
            RENDER_LOG_WARN("GL3Renderer - selected shader does not have a uniform: %s",
                            kGL_ShaderUniformColorBlendMode);
        }
        //  generate vertex arrays and buffers for bitmap draws
        glGenBuffers(2, _bitmapVBO);
        
        glGenVertexArrays(1, &_bitmapVAO);
        glBindVertexArray(_bitmapVAO);

        glBindBuffer(GL_ARRAY_BUFFER, _bitmapVBO[0]);
        glBufferData(GL_ARRAY_BUFFER, kQuadVertexCount*2*sizeof(GLfloat), NULL, GL_STREAM_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrPos);
        glVertexAttribPointer(kGL_ShaderVertexAttrPos, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, _bitmapVBO[1]);
        glBufferData(GL_ARRAY_BUFFER, kQuadVertexCount*2*sizeof(GLfloat), NULL, GL_STREAM_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrUVs);
        glVertexAttribPointer(kGL_ShaderVertexAttrUVs, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindVertexArray(0);

        //  streaming text buffer
        /** @todo Make vec2 cache configurable? */
        _vertCache.reserve(kTextCacheCharCount*kTriQuadVertexCount);
        _uvCache.reserve(kTextCacheCharCount*kTriQuadVertexCount);
        _colorCache.reserve(kTextCacheCharCount*kTriQuadVertexCount);

        glGenBuffers(3, _textVBO);
        
        glGenVertexArrays(1, &_textVAO);
        glBindVertexArray(_textVAO);
        glBindBuffer(GL_ARRAY_BUFFER, _textVBO[0]);
        glBufferData(GL_ARRAY_BUFFER, kTriQuadVertexCount*kTextCacheCharCount*sizeof(glm::vec2),
                     NULL,
                     GL_STREAM_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrPos);
        glVertexAttribPointer(kGL_ShaderVertexAttrPos, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, _textVBO[1]);
        glBufferData(GL_ARRAY_BUFFER, kTriQuadVertexCount*kTextCacheCharCount*sizeof(glm::vec2),
                     NULL,
                     GL_STREAM_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrUVs);
        glVertexAttribPointer(kGL_ShaderVertexAttrUVs, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, _textVBO[2]);
        glBufferData(GL_ARRAY_BUFFER, kTriQuadVertexCount*kTextCacheCharCount*sizeof(glm::vec4),
                     NULL,
                     GL_STREAM_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrColor);
        glVertexAttribPointer(kGL_ShaderVertexAttrColor, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindVertexArray(0);

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
        _shaderLibrary.unloadProgram(_standardShader);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &_textVAO);
        glDeleteVertexArrays(1, &_bitmapVAO);
        glDeleteBuffers(3, _textVBO);
        glDeleteBuffers(2, _bitmapVBO);

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
        return std::move(texture);
    }

    unique_ptr<Texture> GL3Renderer::createTextureFromBuffer(uint16_t w, uint16_t h,
            cinek_pixel_format format,
            const uint8_t* bytes)
    {
        unique_ptr<Texture> texture(getAllocator().newItem<GL3Texture>(*this, w, h, format, bytes), getAllocator());
        return std::move(texture);
    }
    
    void GL3Renderer::begin()
    {
        SDL_GL_MakeCurrent(_window, _glContext);
        glUniformMatrix4fv(_projectionMatUniform, 1, GL_FALSE, glm::value_ptr(_projectionMat));
    }
        
    void GL3Renderer::display()
    {
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
        const Font* font = getFontLibrary().getFont(style.textFont);
        if (!font)
            return;

        const GL3Texture& texture = static_cast<const GL3Texture&>(font->getTexture());
        const RGBAColor& color = style.textColor;
        glm::vec4 colorF = glm::vec4(color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f);

        // force a texture bind on the next bitmap atlas call
        // going to bind the font texture for rendering
        _setAtlasGLTexture = true;      

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.getTextureId());
        glUniform1i(_texSamplerUniform, 0);
        glUniform1i(_colorBlendMode, 1);
        glUniform1i(_vertexColorOn, 1);

        //  setup color uniform for text shader
        
        //  push character verts and uvs to our streaming buffer,
        //  drawing and flushing when the buffer fills
        const size_t kDrawLimit = 65536;        // sanity limit
        _vertCache.clear();
        _uvCache.clear();
        _colorCache.clear();

        glBindVertexArray(_textVAO);

        const char* curtext = text;
        int32_t ox = x;
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
                if (c=='\0' || _vertCache.size() == _vertCache.capacity())
                {
                    // flush our cache - i.e. draw it
                    size_t nv = _vertCache.size();
                    size_t nuv = _uvCache.size();
                    size_t nc = _colorCache.size();
                    const glm::vec2* verts = _vertCache.data();
                    const glm::vec2* uvs = _uvCache.data();
                    const glm::vec4* colors = _colorCache.data();
                    glBindBuffer(GL_ARRAY_BUFFER, _textVBO[0]);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, nv*sizeof(glm::vec2), (GLvoid*)verts);
                    glBindBuffer(GL_ARRAY_BUFFER, _textVBO[1]);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, nuv*sizeof(glm::vec2), (GLvoid*)uvs);
                    glBindBuffer(GL_ARRAY_BUFFER, _textVBO[2]);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, nc*sizeof(glm::vec4), (GLvoid*)colors);
                    glDrawArrays(GL_TRIANGLES, 0, nv);
                    if (c == '\0')
        /* BREAK */     break;
                }

                const stbtt_bakedchar& bakedChar = font->getChar(c);
                /* @todo Insert our uvs from a GLFont instead of using stbtt_bakedchar */
                _uvCache.emplace_back(bakedChar.x0/(float)texture.width(), bakedChar.y1/(float)texture.height());
                _uvCache.emplace_back(bakedChar.x0/(float)texture.width(), bakedChar.y0/(float)texture.height());
                _uvCache.emplace_back(bakedChar.x1/(float)texture.width(), bakedChar.y1/(float)texture.height());
                _uvCache.emplace_back(bakedChar.x1/(float)texture.width(), bakedChar.y1/(float)texture.height());
                _uvCache.emplace_back(bakedChar.x0/(float)texture.width(), bakedChar.y0/(float)texture.height());
                _uvCache.emplace_back(bakedChar.x1/(float)texture.width(), bakedChar.y0/(float)texture.height());
                _vertCache.emplace_back((float)(x+bakedChar.xoff), (float)(y+bakedChar.yoff+bakedChar.y1-bakedChar.y0));
                _vertCache.emplace_back((float)(x+bakedChar.xoff), (float)(y+bakedChar.yoff));
                _vertCache.emplace_back((float)(x+bakedChar.xoff+bakedChar.x1-bakedChar.x0), 
                                        (float)(y+bakedChar.yoff+bakedChar.y1-bakedChar.y0));
                _vertCache.emplace_back((float)(x+bakedChar.xoff+bakedChar.x1-bakedChar.x0), 
                                        (float)(y+bakedChar.yoff+bakedChar.y1-bakedChar.y0));
                _vertCache.emplace_back((float)(x+bakedChar.xoff), (float)(y+bakedChar.yoff));
                _vertCache.emplace_back((float)(x+bakedChar.xoff+bakedChar.x1-bakedChar.x0),
                                        (float)(y+bakedChar.yoff));
                _colorCache.emplace_back(colorF);
                _colorCache.emplace_back(colorF);
                _colorCache.emplace_back(colorF);
                _colorCache.emplace_back(colorF);
                _colorCache.emplace_back(colorF);
                _colorCache.emplace_back(colorF);
                x += bakedChar.xadvance;
            }
            ++curtext;
        }
        while ((curtext - text) < kDrawLimit);
        
        glBindVertexArray(0);
    }

    void GL3Renderer::setBitmapAtlas(cinek_bitmap_atlas atlas)
    {
        _currentAtlas = getBitmapLibrary().getAtlas(atlas);
        if (!_currentAtlas)
            return;
        _setAtlasGLTexture = true;
    }

    void GL3Renderer::drawBitmapFromAtlas(cinek_bitmap_index bitmapIndex, 
                                          int32_t x, int32_t y, float alpha)
    {
        if (!_currentAtlas)
            return;

        /** 
         * @todo Have a GLBitmapInfo that contains a VAO and VBOs for local position and UVs.
         * Shouldn't need to stream vertex data, nor translate BitmapInfo coords into vertex
         * buffer data.  We should only need to bind the VAO and uniform world position
         * before drawing the bitmap.
         */
        const glx::BitmapInfo* bitmapInfo = _currentAtlas->getBitmapFromIndex(bitmapIndex);
        if (!bitmapInfo)
        {
            RENDER_LOG_WARN("GL3Renderer.drawBitmapFromAtlas - bitmap %d not found in current atlas", bitmapIndex);
            return;
        }
        //  suboptimal - see todo.
        const GL3Texture& texture = static_cast<const GL3Texture&>(_currentAtlas->getTexture());
        glm::vec2 bitmapVerts[4];
        glm::vec2 bitmapUVs[4];
        
        bitmapUVs[0].x = bitmapInfo->x/(float)texture.width();
        bitmapUVs[0].y = (bitmapInfo->y+bitmapInfo->h)/(float)texture.height();
        bitmapUVs[1].x = bitmapInfo->x/(float)texture.width();
        bitmapUVs[1].y = bitmapInfo->y/(float)texture.height();
        bitmapUVs[2].x = (bitmapInfo->x+bitmapInfo->w)/(float)texture.width();
        bitmapUVs[2].y = bitmapUVs[0].y;
        bitmapUVs[3].x = bitmapUVs[2].x;
        bitmapUVs[3].y = bitmapUVs[1].y;
        
        bitmapVerts[0].x = x + bitmapInfo->offX;
        bitmapVerts[0].y = y - bitmapInfo->srcH + bitmapInfo->offY + bitmapInfo->h;
        bitmapVerts[1].x = bitmapVerts[0].x;
        bitmapVerts[1].y = y - bitmapInfo->srcH + bitmapInfo->offY;
        bitmapVerts[2].x = bitmapVerts[0].x + bitmapInfo->w;
        bitmapVerts[2].y = bitmapVerts[0].y;
        bitmapVerts[3].x = bitmapVerts[2].x;
        bitmapVerts[3].y = bitmapVerts[1].y;

        if (_setAtlasGLTexture)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture.getTextureId());
            glUniform1i(_texSamplerUniform, 0 /*GL_TEXTURE0*/);
            _setAtlasGLTexture = false;
        }

        glUniform1i(_colorBlendMode, 0);
        glUniform1i(_vertexColorOn, 0);
        
        glBindVertexArray(_bitmapVAO);
        glBindBuffer(GL_ARRAY_BUFFER, _bitmapVBO[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, kQuadVertexCount*2*sizeof(GLfloat), (GLvoid*)bitmapVerts);
        glBindBuffer(GL_ARRAY_BUFFER, _bitmapVBO[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, kQuadVertexCount*2*sizeof(GLfloat), (GLvoid*)bitmapUVs);

        /**
         * @todo Perhaps a GL_TRIANGLE_STRIP instead of two triangles?
         */
        glDrawArrays(GL_TRIANGLE_STRIP, 0, kQuadVertexCount);
        glBindVertexArray(0);
    }
        
    }   // namespace glx
}   // namespace cinekine
