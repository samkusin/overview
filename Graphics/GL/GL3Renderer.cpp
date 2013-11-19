//
//  GL/GL3Renderer.cpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "GL3Renderer.hpp"
#include "GL3Texture.hpp"

#include "../RenderDebug.hpp"

namespace cinekine {
    namespace glx {

    GL3Renderer::GL3Renderer(const RendererInitParameters& initParams,
                             SDL_Window* window,
                             const Allocator& allocator) :
        Renderer(initParams, allocator),
        _window(window),
        _glContext(NULL),
        _shaderLibrary("static/shaders", allocator),
        _currentAtlas(kCinekBitmapAtlas_Invalid),
        _standardShader(0)
    {
        _glContext = SDL_GL_CreateContext(_window);
        if (!_glContext)
        {
            RENDER_LOG_ERROR("GL3Renderer: failed to obtain a GL context: %s", SDL_GetError());
            return;
        }
        _standardShader = _shaderLibrary.loadProgram("standard.json");
    }
    
    GL3Renderer::~GL3Renderer()
    {
        _shaderLibrary.unloadProgram(_standardShader);
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
            const uint8_t* bytes, uint16_t stride)
    {
        unique_ptr<Texture> texture(getAllocator().newItem<GL3Texture>(*this, w, h, format, bytes, stride), getAllocator());
        return std::move(texture);
    }
    
    void GL3Renderer::begin()
    {
        
    }
        
    void GL3Renderer::end()
    {
        SDL_GL_SwapWindow(_window);
    }

    Rect GL3Renderer::getViewport() const
    {
        return Rect(0,0,1,1);
    }

    void GL3Renderer::clear(const RGBAColor& color)
    {
        
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
        /*

        SDL_Renderer* sdlRenderer = _renderer;

        const SDLTexture& texture = static_cast<const SDLTexture&>(font->getTexture());
        SDL_Texture* sdlTexture = texture.getSDLTexture();
        SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);
        const RGBAColor& color = style.textColor;
        SDL_SetTextureColorMod(sdlTexture, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(sdlTexture, color.a);
    
        //const int32_t kOX = x;      // used for tab stops
        const char* curtext = text;
        
        while (*curtext)
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
                const stbtt_bakedchar& bakedChar = font->getChar(c);
                SDL_Rect src;
                SDL_Rect dest;
                
                src.x = bakedChar.x0;
                src.y = bakedChar.y0;
                src.w = bakedChar.x1 - bakedChar.x0;
                src.h = bakedChar.y1 - bakedChar.y0;
                dest.x = x + bakedChar.xoff;
                dest.y = y + bakedChar.yoff;
                dest.w = src.w;
                dest.h = src.h;
                
                SDL_RenderCopy(sdlRenderer, sdlTexture, &src, &dest);
                x += bakedChar.xadvance;
            }
            ++curtext;
        }
        */
    }

    void GL3Renderer::setBitmapAtlas(cinek_bitmap_atlas atlas)
    {
        _currentAtlas = atlas;
    }

    void GL3Renderer::drawBitmapFromAtlas(cinek_bitmap_index bitmapIndex, 
                                          int32_t x, int32_t y, float alpha)
    {
        const BitmapAtlas* atlas = getBitmapLibrary().getAtlas(_currentAtlas); 
        if (!atlas)
            return;
        /*
        const SDLTexture& texture = (SDLTexture&)atlas->getTexture();
        SDL_Texture* sdlTexture = texture.getSDLTexture();
        const glx::BitmapInfo* bitmapInfo = atlas->getBitmapFromIndex(bitmapIndex);
        if (bitmapInfo)
        {
            SDL_Rect srcRect;
            SDL_Rect destRect;
            srcRect.x = bitmapInfo->x;
            srcRect.y = bitmapInfo->y;
            srcRect.w = bitmapInfo->w;
            srcRect.h = bitmapInfo->h;
            destRect.x = x + bitmapInfo->offX;
            destRect.y = y - bitmapInfo->srcH + bitmapInfo->offY;
            destRect.w = bitmapInfo->w;
            destRect.h = bitmapInfo->h;
            SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);
            SDL_SetTextureColorMod(sdlTexture, 255, 255, 255);
            SDL_SetTextureAlphaMod(sdlTexture, (uint8_t)(alpha*255));
            SDL_RenderCopy(_renderer, sdlTexture, &srcRect, &destRect);
        }
        */
    }
        
    }   // namespace glx
}   // namespace cinekine
