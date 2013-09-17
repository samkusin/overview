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

#include "SDL_render.h"

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

    void Stylus::setFont(uint32_t fontIndex)
    {

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

    }   // namespace ovengine
}   // namespace cinekine

