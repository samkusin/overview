//
//  SDLTexture.h
//  Overview
//
//  Created by Samir Sinha on 9/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_SDL_Texture_hpp
#define CK_Graphics_SDL_Texture_hpp

#include "../Texture.hpp"

#include "cinek/cpp/ckdefs.hpp"

#include "SDL2/SDL_render.h"

namespace cinekine {
    namespace glx {
    
    class Renderer;
    
    class SDLTexture: public Texture
    {
        CK_CLASS_NON_COPYABLE(SDLTexture);

    public:
        SDLTexture(Renderer& renderer, const char *pathname);
        SDLTexture(Renderer& renderer, uint16_t w, uint16_t h,
            cinek_pixel_format format,
            const uint8_t* bytes, uint16_t stride);
        SDLTexture(SDLTexture&& other);
        virtual ~SDLTexture();

        virtual operator bool() const {
            return _texture!=NULL;
        }
        
        SDL_Texture* getSDLTexture() const {
            return _texture;
        }
        
    private:
        Renderer& _renderer;
        SDL_Texture* _texture;
    };
        
    }   // namespace glx
}   // namespace cinekine

#endif