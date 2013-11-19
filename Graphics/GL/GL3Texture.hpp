//
//  GL/GL3Texture.hpp
//  Overview
//
//  Created by Samir Sinha on 11/16/2013.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_GL_GL3_Texture_hpp
#define CK_Graphics_GL_GL3_Texture_hpp

#include "../Texture.hpp"

#include "GLUtils.hpp"

namespace cinekine {
    namespace glx {
    
    class Renderer;
    
    class GL3Texture: public Texture
    {
        CK_CLASS_NON_COPYABLE(GL3Texture);

    public:
        GL3Texture(Renderer& renderer, const char *pathname);
        GL3Texture(Renderer& renderer, uint16_t w, uint16_t h,
            cinek_pixel_format format,
            const uint8_t* bytes, uint16_t stride);
        GL3Texture(GL3Texture&& other);
        virtual ~GL3Texture();

        virtual operator bool() const {
            return _texture!=GL_INVALID_VALUE;
        }

    private:
        Renderer& _renderer;
        GLuint _texture;

    };
        
    }   // namespace glx
}   // namespace cinekine

#endif