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
                 SDL_Window* window, const InitParameters& initParams,
                 const Allocator& allocator);
        ~Renderer();
        
        void begin();
        void end();
        
        
        //  returns subsystems.
        const BitmapLibrary& getBitmapLibrary() const {
            return _bitmapLibrary;
        }
        BitmapLibrary& getBitmapLibrary() {
            return _bitmapLibrary;
        }
        
        //  returns the current viewport rect
        SDL_Rect getViewport() const;
        
        //  Supporting methods
        Allocator& getAllocator() {
            return _allocator;
        }
        Theater& getTheater() {
            return _theater;
        }
        
        // Resource Management
        //
        //  Creates a texture
        unique_ptr<Texture> loadTexture(const char* pathname);
        
        // Drawing
        //
        //  Draws a bitmap onto the current screen.
        void drawBitmap(const cinek_bitmap& bitmap, int32_t x, int32_t y);
        
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
        
        //  rendering state (TODO, split into a separate object?)
        cinek_bitmap_atlas _currentAtlasIndex;
        const BitmapAtlas* _currentAtlas;
    };
        
        
    }   // namespace ovengine
}   // namespace cinekine

#endif
