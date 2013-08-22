//
//  Renderer.cpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "Renderer.hpp"
#include "Engine/Defines.hpp"

namespace cinekine {
    namespace ovengine {

    Renderer::Renderer(SDL_Window* window, const InitParameters& initParams,
                       const Allocator& allocator) :
        _allocator(allocator),
        _renderer(NULL),
        _bitmapLibrary(*this, initParams.bitmapAtlasDir)
    {
        _renderer = SDL_CreateRenderer(window, -1,
                                       SDL_RENDERER_ACCELERATED |
                                       SDL_RENDERER_PRESENTVSYNC);
    }
    
    Renderer::~Renderer()
    {
        SDL_DestroyRenderer(_renderer);
    }
    
    void Renderer::begin()
    {
        SDL_RenderClear(_renderer);
    }
        
    void Renderer::end()
    {
        SDL_RenderPresent(_renderer);
    }
        
    }   // namespace ovengine
}   // namespace cinekine
