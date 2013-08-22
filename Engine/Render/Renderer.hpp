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
        
        Renderer(SDL_Window* window, const InitParameters& initParams,
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

        //  returns the SDL renderer object (owned by the Renderer.)
        SDL_Renderer* getSDLRenderer() {
            return _renderer;
        }
        
        Allocator& getAllocator() {
            return _allocator;
        }

    private:
        Allocator _allocator;
        SDL_Renderer* _renderer;
        BitmapLibrary _bitmapLibrary;
    };
        
        
    }   // namespace ovengine
}   // namespace cinekine

#endif
