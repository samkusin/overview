//
//  SDLTexture.cpp
//  Overview
//
//  Created by Samir Sinha on 9/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "SDLTexture.hpp"
// TODO - move into SDL/SDLRenderer.hpp
#include "../Renderer.hpp"

#include "SDL_image.h"

namespace cinekine {
    namespace ovengine {
        
      
    SDLTexture::SDLTexture(Renderer& renderer, const char* pathname) :
        _renderer(renderer),
        _texture(NULL)
    {
        _texture =  IMG_LoadTexture(_renderer.getSDLRenderer(), pathname);
    }
    
    SDLTexture::~SDLTexture()
    {
        if (_texture)
        {
            SDL_DestroyTexture(_texture);
        }
    }
    
    SDLTexture::SDLTexture(SDLTexture&& other) :
        _renderer(other._renderer),
        _texture(std::move(other._texture))
    {
        other._texture = NULL;
    }
        
    }   // namespace ovengine
}   // namespace cinekine