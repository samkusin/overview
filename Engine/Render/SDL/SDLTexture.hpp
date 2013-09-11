//
//  SDLTexture.h
//  Overview
//
//  Created by Samir Sinha on 9/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Renderer_SDL_Texture_hpp
#define Overview_Renderer_SDL_Texture_hpp

#include "../Texture.hpp"

#include "SDL_render.h"

namespace cinekine {
    namespace ovengine {
    
    class Renderer;
    
    class SDLTexture: public Texture
    {
        CK_CLASS_NON_COPYABLE(SDLTexture);

    public:
        SDLTexture(Renderer& renderer, const char *pathname);
        SDLTexture(SDLTexture&& other);
        virtual ~SDLTexture();
        
        SDL_Texture* getSDLTexture() const {
            return _texture;
        }
        
    private:
        Renderer& _renderer;
        SDL_Texture* _texture;
    };
        
    }   // namespace ovengine
}   // namespace cinekine

#endif