//
//  Renderer.cpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "SDLRenderer.hpp"
#include "SDLTexture.hpp"

namespace cinekine {
    namespace glx {

    SDLRenderer::SDLRenderer(const RendererInitParameters& initParams,
                       SDL_Window* window,
                       const Allocator& allocator) :
        Renderer(initParams, window, allocator),
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
            const uint8_t* bytes, uint16_t stride)
    {
        unique_ptr<Texture> texture(getAllocator().newItem<SDLTexture>(*this, w, h, format, bytes, stride), getAllocator());
        return std::move(texture);
    }
    
    void SDLRenderer::begin()
    {
        SDL_RenderClear(_renderer);
    }
        
    void SDLRenderer::end()
    {
        SDL_RenderPresent(_renderer);
    }
        
    }   // namespace glx
}   // namespace cinekine
