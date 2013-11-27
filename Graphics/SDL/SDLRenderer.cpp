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
        _renderer(NULL)
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

    void SDLRenderer::drawTextureRect(const Texture& texture,
                                      const Rect& source, const Rect& dest,
                                      const RGBAColor& color)
    {
        SDL_Texture* sdlTexture = ((const SDLTexture& )texture).getSDLTexture();
        if (sdlTexture)
        {
            SDL_Rect srcRect;
            SDL_Rect destRect;
            srcRect.x = source.left;
            srcRect.y = source.top;
            srcRect.w = source.width();
            srcRect.h = source.height();
            destRect.x = dest.left;
            destRect.y = dest.top;
            destRect.w = dest.width();
            destRect.h = dest.height();
            SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);
            SDL_SetTextureColorMod(sdlTexture, color.r, color.g, color.b);
            SDL_SetTextureAlphaMod(sdlTexture, color.a);
            SDL_RenderCopy(_renderer, sdlTexture, &srcRect, &destRect);
        }      
    }

    }   // namespace glx
}   // namespace cinekine
