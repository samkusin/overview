//
//  GL/GL3Renderer.hpp
//  Graphics
//
//  Created by Samir Sinha on 11/16/2013.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_GL_GL3Renderer_hpp
#define CK_Graphics_GL_GL3Renderer_hpp

#include "Graphics/Renderer.hpp"
#include "GLShaderLibrary.hpp"

#include "SDL2/SDL_video.h"

namespace cinekine {
    namespace glx {

    //  The OpenGL 3.x Renderer
    //
    class GL3Renderer: public Renderer
    {
    public:    
        GL3Renderer(const RendererInitParameters& initParams,
                 SDL_Window* window,
                 const Allocator& allocator);
        virtual ~GL3Renderer();
        
        virtual operator bool() const {
            return _glContext != NULL;
        }
        
        ///////////////////////////////////////////////////////////////////////
        // Resource Management
        //
        virtual unique_ptr<Texture> loadTexture(const char* pathname);

        virtual unique_ptr<Texture> createTextureFromBuffer(uint16_t w, uint16_t h,
            cinek_pixel_format format,
            const uint8_t* bytes, uint16_t stride);

        ///////////////////////////////////////////////////////////////////////
        //  Housekeeping methods, used to mark the beginning and end of a render
        //  frame.
        //  
        virtual void begin();
        virtual void end();
        virtual Rect getViewport() const;
    
        ///////////////////////////////////////////////////////////////////////
        //  Renderer Drawing Methods
        //  All rendering methods act on the current target.
        //  
        virtual void clear(const RGBAColor& color);
        virtual void drawRect(const Rect& rect, const Style& style);
        virtual void drawRoundedRect(const Rect& rect, const std::array<int32_t, 4>& radii,
                                     const Style& style);
        virtual void drawText(const char* text, int32_t x, int32_t y,
                              const Style& style);
        virtual void setBitmapAtlas(cinek_bitmap_atlas atlas);

        virtual void drawBitmapFromAtlas(cinek_bitmap_index bitmapIndex, 
                                         int32_t x, int32_t y, float alpha);

    private:
        SDL_Window* _window;
        SDL_GLContext _glContext;
        GLShaderLibrary _shaderLibrary;

        cinek_bitmap_atlas _currentAtlas;
        GLuint _standardShader;
    };
    

    }   // namespace glx
}   // namespace cinekine

#endif
