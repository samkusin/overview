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
#include "Rect.hpp"

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

    /**
     * @class Renderer
     * @brief The base interface for all platform-specific rendering
     *        implementations.
     */
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
        /**
         * Clears the entire client region (entire screen if fullscreen)
         * @param color A RGBA color value.
         */
        virtual void clear(const RGBAColor& color) = 0;
        /**
         * [drawTextureRect description]
         * @param texture [description]
         * @param source  [description]
         * @param dest    [description]
         * @param color   [description]
         */
        virtual void drawTextureRect(const Texture& texture,
                                     const Rect& source, const Rect& dest,
                                     const RGBAColor& color) = 0;


    private:
        Allocator _allocator;
    };
    

    }   // namespace glx
}   // namespace cinekine

#endif
