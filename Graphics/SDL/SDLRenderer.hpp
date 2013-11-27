//
//  SDL/Renderer.hpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_SDL_Renderer_hpp
#define CK_Graphics_SDL_Renderer_hpp

#include "Graphics/Renderer.hpp"

#include "SDL2/SDL_render.h"

namespace cinekine {
    namespace glx {
    
    //  The top-level Renderer object
    //      The Renderer handles drawing to the display.
    class SDLRenderer: public Renderer
    {
    public:        
        SDLRenderer(const RendererInitParameters& initParams,
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
            const uint8_t* bytes);
        ///////////////////////////////////////////////////////////////////////
        //  Renderer Draw State
        //
        virtual void begin();
        virtual void display();
        virtual Rect getViewport() const;
        virtual void setViewport(const Rect& rect);

        //  Specialized clear method, which clears the current rendering target
        virtual void clear(const RGBAColor& color);
    
        virtual void drawTextureRect(const Texture& texture,
                                     const Rect& source, const Rect& dest,
                                     const RGBAColor& color);

        ///////////////////////////////////////////////////////////////////////
        //  Renderer Implementation Specific Accessors
        //  Access to SDL system renderer
        SDL_Renderer* getSDLRenderer() const {
            return _renderer;
        }

    private:
        SDL_Renderer* _renderer;
    };
    

    }   // namespace glx
}   // namespace cinekine

#endif
