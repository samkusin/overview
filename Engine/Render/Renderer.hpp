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

#include "SDL_video.h"
#include "SDL_render.h"

namespace cinekine {
    namespace ovengine {
    
    class Theater;
    class Texture;
    
    //  The top-level Renderer object
    //      The Renderer handles drawing to the display.
    class Renderer
    {
    public:
        //  Extra parameters for initializing the renderer.
        struct InitParameters
        {
            const char* bitmapAtlasDir;
        };
        
        Renderer(Theater& theater,
                 const InitParameters& initParams,
                 SDL_Window* window,
                 const Allocator& allocator);
        virtual ~Renderer();
        
        operator bool() const {
            return _renderer != NULL;
        }
        
        ///////////////////////////////////////////////////////////////////////
        //  returns supersystems
        const Theater& getTheater() const {
            return _theater;
        }
        Theater& getTheater() {
            return _theater;
        }
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
        //  To be moved to subclass of Renderer when needed (SDL version)
        //  Access to SDL system renderer
        SDL_Renderer* getSDLRenderer() const {
            return _renderer;
        }

    private:
        Allocator _allocator;
        Theater& _theater;
        SDL_Window* _mainWindow;
        SDL_Renderer* _renderer;
        SDL_Rect _viewRect;
        BitmapLibrary _bitmapLibrary;
        FontLibrary _fontLibrary;
    };
    

    }   // namespace ovengine
}   // namespace cinekine

#endif
