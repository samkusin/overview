//
//  Renderer.hpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Renderer_hpp
#define Overview_Renderer_hpp

#include "BitmapLibrary.hpp"
#include "FontLibrary.hpp"

#include "cinek/cpp/ckalloc.hpp"
#include "cinek/cpp/ckstring.hpp"

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
                 SDL_Window* window,
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
            const uint8_t* bytes, uint16_t stride) = 0;
        ///////////////////////////////////////////////////////////////////////
        //  Renderer Draw State
        //
        virtual void begin() = 0;
        virtual void end() = 0;

    private:
        Allocator _allocator;
        SDL_Window* _mainWindow;
        SDL_Rect _viewRect;
        BitmapLibrary _bitmapLibrary;
        FontLibrary _fontLibrary;
    };
    

    }   // namespace glx
}   // namespace cinekine

#endif
