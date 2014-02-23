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

#include <cinek/framework/types.hpp>

#include "SDL2/SDL_render.h"

namespace cinekine {
    namespace glx {
    
    class Renderer;
    
    class SDLTexture: public Texture
    {
        CK_CLASS_NON_COPYABLE(SDLTexture);

    public:
        SDLTexture(Renderer& renderer, const char *pathname);
        SDLTexture(Renderer& renderer, uint32_t w, uint32_t h,
            cinek_pixel_format format,
            const uint8_t* bytes);
        SDLTexture(SDLTexture&& other);
        virtual ~SDLTexture();

        virtual operator bool() const {
            return _texture!=NULL;
        }
        
        SDL_Texture* getSDLTexture() const {
            return _texture;
        }

        virtual uint32_t width() const {
            return _width;
        }
        virtual uint32_t height() const {
            return _height;
        }
     
    private:
        Renderer& _renderer;
        SDL_Texture* _texture;
        // while we could use SDL_QueryTexture, these values are cached for
        // to prevent frequent calls to SDL
        uint32_t _width, _height;
    };
        
    }   // namespace glx
}   // namespace cinekine

#endif