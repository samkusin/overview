//
//  Renderer.hpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_SDL_SDLRenderer_hpp
#define Overview_SDL_SDLRenderer_hpp

#include "../Renderer.hpp"

#include "SDL2/SDL_render.h"

namespace cinekine {
    namespace ovengine {
    
    //  The top-level Renderer object
    //      The Renderer handles drawing to the display.
    class SDLRenderer: public Renderer
    {
    public:        
        SDLRenderer(const Renderer::InitParameters& initParams,
                 SDL_Window* window,
                 const Allocator& allocator);
        virtual ~SDLRenderer();
        
        operator bool() const {
            return _renderer != NULL;
        }
        
        ///////////////////////////////////////////////////////////////////////
        // Resource Management
        //
        //  Creates a renderable texture from a file source
        unique_ptr<Texture> loadTexture(const char* pathname);
        //  Creates a renderable texture from a byte array.  The width and height
        //  and stride are *source* buffer values.  The final texture width, height
        //  and format depend on the Renderer implementation and platform.
        unique_ptr<Texture> createTextureFromBuffer(uint16_t w, uint16_t h,
            cinek_pixel_format format,
            const uint8_t* bytes, uint16_t stride);
        ///////////////////////////////////////////////////////////////////////
        //  Renderer Draw State
        //
        void begin();
        void end();

        ///////////////////////////////////////////////////////////////////////
        //  Renderer Implementation Specific Accessors
        //  Access to SDL system renderer
        SDL_Renderer* getSDLRenderer() const {
            return _renderer;
        }

    private:
        SDL_Renderer* _renderer;
    };
    

    }   // namespace ovengine
}   // namespace cinekine

#endif
