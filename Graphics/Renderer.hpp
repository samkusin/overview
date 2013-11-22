//
//  Renderer.hpp
//  Graphics
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_Renderer_hpp
#define CK_Graphics_Renderer_hpp

#include "RenderTypes.hpp"
#include "BitmapLibrary.hpp"
#include "FontLibrary.hpp"
#include "Rect.hpp"
#include "Style.hpp"

#include "cinek/cpp/ckalloc.hpp"

#include "SDL2/SDL_video.h"


namespace cinekine {
    namespace glx {
    
    class Theater;
    class Texture;

    //  Extra parameters for initializing the renderer.
    struct RendererInitParameters
    {
        const char* bitmapAtlasDir;
    };

    //  The top-level Renderer object
    //      The Renderer handles drawing to the display.
    class Renderer
    {
    public:    
        Renderer(const RendererInitParameters& initParams,
                 const Allocator& allocator);
        virtual ~Renderer();
        
        virtual operator bool() const = 0;
        
        ///////////////////////////////////////////////////////////////////////
        //  returns supersystems
        const Allocator& getAllocator() const {
            return _allocator;
        }
        Allocator& getAllocator() {
            return _allocator;
        }
        ///////////////////////////////////////////////////////////////////////
        //  returns subsystems.
        const BitmapLibrary& getBitmapLibrary() const {
            return _bitmapLibrary;
        }
        BitmapLibrary& getBitmapLibrary() {
            return _bitmapLibrary;
        }
        const FontLibrary& getFontLibrary() const {
            return _fontLibrary;
        }
        FontLibrary& getFontLibrary() {
            return _fontLibrary;
        }

        ///////////////////////////////////////////////////////////////////////
        // Resource Management
        //
        //  Creates a renderable texture from a file source
        virtual unique_ptr<Texture> loadTexture(const char* pathname) = 0;
        //  Creates a renderable texture from a byte array.  The width and height
        //  and stride are *source* buffer values.  The final texture width, height
        //  and format depend on the Renderer implementation and platform.
        virtual unique_ptr<Texture> createTextureFromBuffer(uint16_t w, uint16_t h,
            cinek_pixel_format format,
            const uint8_t* bytes) = 0;
  
        /**
         *  Call at the start of rendering a frame
         */
        virtual void begin() = 0;
        /**
         *  Display the rendered frame
         */
        virtual void display() = 0;
        /**
         * @return Returns the current viewport rect
         */
        virtual Rect getViewport() const = 0;
        /**
         * Sets the current rendering viewport
         * @param rect Rectangle using client coordinates (i.e. the window's client area)
         */
        virtual void setViewport(const Rect& rect) = 0;

        ///////////////////////////////////////////////////////////////////////
        //  Renderer Drawing Methods
        //  All rendering methods act on the current target.
        //  
        //  Specialized clear method, which clears the current rendering target
        virtual void clear(const RGBAColor& color) = 0;
        //  Draws a rectangle
        virtual void drawRect(const Rect& rect, const Style& style) = 0;
        //  Draws a rectangle with four rounded corners (left-top, right-top,
        //      right-bottom, left-bottom)
        virtual void drawRoundedRect(const Rect& rect, const std::array<int32_t, 4>& radii,
                                     const Style& style) = 0;
        //  Draws text
        virtual void drawText(const char* text, int32_t x, int32_t y,
                              const Style& style) = 0;
        //  Sets the current bitmap atlas
        virtual void setBitmapAtlas(cinek_bitmap_atlas atlas) = 0;
        //  Draws a bitmap from the current atlas with alpha
        virtual void drawBitmapFromAtlas(cinek_bitmap_index bitmapIndex, 
                                         int32_t x, int32_t y, float alpha) = 0;

    private:
        Allocator _allocator;
        BitmapLibrary _bitmapLibrary;
        FontLibrary _fontLibrary;
    };
    

    }   // namespace glx
}   // namespace cinekine

#endif
