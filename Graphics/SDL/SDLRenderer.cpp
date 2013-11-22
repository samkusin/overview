//
//  Renderer.cpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "SDLRenderer.hpp"
#include "SDLTexture.hpp"

#include "SDL2/SDL_rect.h"

namespace cinekine {
    namespace glx {

    SDLRenderer::SDLRenderer(const RendererInitParameters& initParams,
                       SDL_Window* window,
                       const Allocator& allocator) :
        Renderer(initParams, allocator),
        _renderer(NULL),
        _currentAtlas()
    {
        _renderer = SDL_CreateRenderer(window, -1,
                                       SDL_RENDERER_ACCELERATED |
                                       SDL_RENDERER_PRESENTVSYNC);
    }
    
    SDLRenderer::~SDLRenderer()
    {
        SDL_DestroyRenderer(_renderer);
    }

    unique_ptr<Texture> SDLRenderer::loadTexture(const char* pathname)
    {
        // TODO - perhaps we need a make_unique_ptr (C++11 version) to confirm that the allocator
        //  creating the item is also used to delete the item.
        unique_ptr<Texture> texture(getAllocator().newItem<SDLTexture>(*this, pathname), getAllocator());
        return std::move(texture);
    }

    unique_ptr<Texture> SDLRenderer::createTextureFromBuffer(uint16_t w, uint16_t h,
            cinek_pixel_format format,
            const uint8_t* bytes)
    {
        unique_ptr<Texture> texture(getAllocator().newItem<SDLTexture>(*this, w, h, format, bytes), getAllocator());
        return std::move(texture);
    }
    
    void SDLRenderer::begin()
    {
        SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
    }
        
    void SDLRenderer::display()
    {
        SDL_RenderPresent(_renderer);
    }

    Rect SDLRenderer::getViewport() const
    {
        SDL_Rect viewportRect;
        SDL_RenderGetViewport(_renderer, &viewportRect);
        return Rect(viewportRect.x, viewportRect.y,
                    viewportRect.x + viewportRect.w-1, viewportRect.y + viewportRect.h-1);
    }

    void SDLRenderer::setViewport(const Rect& rect)
    {
        SDL_Rect viewportRect;
        viewportRect.x = rect.left;
        viewportRect.y = rect.top;
        viewportRect.w = rect.width();
        viewportRect.h = rect.height();
        SDL_RenderSetViewport(_renderer, &viewportRect);
    }

    void SDLRenderer::clear(const RGBAColor& color)
    {
        SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(_renderer);
    }

    void SDLRenderer::drawRect(const Rect& rect, const Style& style)
    {
        drawRoundedRect(rect, {0,0,0,0}, style);
    }

    void SDLRenderer::drawRoundedRect(const Rect& rect, const std::array<int32_t, 4>& radii,
                                      const Style& style)
    {
        //  fill rect first, then overlay the border
        SDL_Rect sdlRect;
        sdlRect.x = rect.left;
        sdlRect.y = rect.top;
        sdlRect.w = (rect.right - rect.left)+1;
        sdlRect.h = (rect.bottom - rect.top)+1;
        if (style.fillMethod != kFillMethod_NoFill)
        {
            SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
            SDL_SetRenderDrawBlendMode(_renderer, blendMode);
            SDL_SetRenderDrawColor(_renderer,
                    style.fillColor.r, 
                    style.fillColor.g,
                    style.fillColor.b,
                    style.fillColor.a);
            SDL_RenderFillRect(_renderer, &sdlRect);
        }
        if (style.lineMethod != kLineMethod_NoLine)
        {
            SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
            SDL_SetRenderDrawBlendMode(_renderer, blendMode);
            SDL_SetRenderDrawColor(_renderer,
                style.lineColor.r, 
                style.lineColor.g,
                style.lineColor.b,
                style.lineColor.a);
            SDL_RenderDrawRect(_renderer, &sdlRect); 
        }
    }

    void SDLRenderer::drawText(const char* text, int32_t x, int32_t y,
                               const Style& style)
    {
        const Font* font = getFontLibrary().getFont(style.textFont);
        if (!font)
            return;

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
            /*
                for (int i = 0; i < 4; ++i)
                {
                    if (x < g_tabStops[i]+ox)
                    {
                        x = g_tabStops[i]+ox;
                        break;
                    }
                }
            */
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
    }

    void SDLRenderer::setBitmapAtlas(cinek_bitmap_atlas atlas)
    {
        _currentAtlas = getBitmapLibrary().getAtlas(atlas);
    }

    void SDLRenderer::drawBitmapFromAtlas(cinek_bitmap_index bitmapIndex, 
                                          int32_t x, int32_t y, float alpha)
    {
        if (!_currentAtlas)
            return;

        const BitmapAtlas& atlas = *_currentAtlas.get();
        
        const SDLTexture& texture = (SDLTexture&)atlas.getTexture();
        SDL_Texture* sdlTexture = texture.getSDLTexture();
        const glx::BitmapInfo* bitmapInfo = atlas.getBitmapFromIndex(bitmapIndex);
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
    }
        
    }   // namespace glx
}   // namespace cinekine
