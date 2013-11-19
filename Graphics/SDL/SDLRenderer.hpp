//
//  SDL/Renderer.hpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_SDL_Renderer_hpp
#define CK_Graphics_SDL_Renderer_hpp

#include "../Renderer.hpp"

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
            const uint8_t* bytes, uint16_t stride);
        ///////////////////////////////////////////////////////////////////////
        //  Renderer Draw State
        //
        void begin();
        void end();

        virtual Rect getViewport() const;

        //  Specialized clear method, which clears the current rendering target
        virtual void clear(const RGBAColor& color);
        //  Draws a rectangle
        virtual void drawRect(const Rect& rect, const Style& style);
        //  Draws a rectangle with four rounded corners (left-top, right-top,
        //      right-bottom, left-bottom)
        virtual void drawRoundedRect(const Rect& rect,
                                     const std::array<int32_t, 4>& radii,
                                     const Style& style);
        //  Draws text
        virtual void drawText(const char* text, int32_t x, int32_t y,
                              const Style& style);
        //  Sets the current bitmap atlas
        virtual void setBitmapAtlas(cinek_bitmap_atlas atlas);
        //  Draws a bitmap from the current atlas with alpha
        virtual void drawBitmapFromAtlas(cinek_bitmap_index bitmapIndex, 
                                         int32_t x, int32_t y, float alpha);

        ///////////////////////////////////////////////////////////////////////
        //  Renderer Implementation Specific Accessors
        //  Access to SDL system renderer
        SDL_Renderer* getSDLRenderer() const {
            return _renderer;
        }

    private:
        SDL_Renderer* _renderer;
        cinek_bitmap_atlas _currentAtlas;
    };
    

    }   // namespace glx
}   // namespace cinekine

#endif
