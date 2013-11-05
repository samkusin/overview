//
//  Render/Stylus.cpp
//  Overview
//
//  Created by Samir Sinha on 9/16/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "Stylus.hpp"
#include "Renderer.hpp"
#include "FontLibrary.hpp"
#include "BitmapLibrary.hpp"
#include "SDL/SDLTexture.hpp"

#include "SDL2/SDL_render.h"

namespace cinekine {
    namespace ovengine {    
      
    Stylus::Stylus(Renderer& renderer) :
        _renderer(renderer),
        _atlasIndex(kCinekBitmapAtlas_Invalid),
        _atlas(nullptr),
        _fontHandle(kFontHandle_Invalid)
    {
    }

    Stylus::~Stylus()
    {
    }

    //  returns the current viewport rect
    SDL_Rect Stylus::getViewport() const
    {
        SDL_Rect viewportRect;
        SDL_RenderGetViewport(_renderer.getSDLRenderer(), &viewportRect);
        return viewportRect;
    }

    void Stylus::setBitmapAtlas(cinek_bitmap_atlas atlas)
    {
        if (atlas != _atlasIndex)
        {
            _atlasIndex = atlas;
            _atlas = _renderer.getBitmapLibrary().getAtlas(_atlasIndex);
        }
    }

    void Stylus::drawBitmap(cinek_bitmap_index bitmapIndex, int32_t x, int32_t y)
    {
        if (!_atlas)
            return;
        
        const SDLTexture& texture = (SDLTexture&)_atlas->getTexture();
        SDL_Texture* sdlTexture = texture.getSDLTexture();
        const ovengine::BitmapInfo* bitmapInfo = _atlas->getBitmapFromIndex(bitmapIndex);
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
            SDL_RenderCopy(_renderer.getSDLRenderer(), sdlTexture, &srcRect, &destRect);
        }
    }
    
    void Stylus::setFont(FontHandle fontHandle)
    {
        _fontHandle = fontHandle;
    }

    void Stylus::drawText(const char* text, int32_t x, int32_t y, const SDL_Color& color)
    {
        const Font* font = _renderer.getFontLibrary().getFont(_fontHandle);
        if (!font)
            return;

        SDL_Renderer* sdlRenderer = _renderer.getSDLRenderer();

        const SDLTexture& texture = static_cast<const SDLTexture&>(font->getTexture());
        SDL_Texture* sdlTexture = texture.getSDLTexture();
        SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureColorMod(sdlTexture, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(sdlTexture, color.a);
    
        const int32_t kOX = x;      // used for tab stops
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

    }   // namespace ovengine
}   // namespace cinekine

