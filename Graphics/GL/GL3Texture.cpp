//
//  GL/GL3Texture.cpp
//  Overview
//
//  Created by Samir Sinha on 11/16/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "GL3Texture.hpp"
// TODO - move into SDL/SDLRenderer.hpp
#include "GL3Renderer.hpp"

#include "../RenderDebug.hpp"
#include "../RenderUtils.hpp"

#include "SDL2/SDL_image.h"

namespace cinekine {
    namespace glx {

    struct GLTextureFormat
    {
        GLint internalFormat;
        GLenum format;
        GLenum type;
    };

    bool sdlToGLFormat(GLTextureFormat& glFormat, GLenum sdlPixelFormat)
    {
        // With OpenGL, it's easier to upload 4-byte aligned images
        switch (sdlPixelFormat)
        {
        case SDL_PIXELFORMAT_ARGB8888:              // high order alpha, low order blue, 8-bit comp
            glFormat.internalFormat = GL_RGBA8;     // stored with 8-bit components, no compression
            glFormat.format = GL_BGRA;              // source data packing, and ordering
            glFormat.type = GL_UNSIGNED_INT_8_8_8_8_REV;
            break;
        case SDL_PIXELFORMAT_ARGB1555:              // high order alpha (1-bit), low order blue, 5-bit
            glFormat.internalFormat = GL_RGB5_A1;   // store as a 1555 texture
            glFormat.format = GL_BGRA;              // source data as a 1555 texture
            glFormat.type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
            break;
        default:
            return false;
        }

        return true;
    }


    GL3Texture::GL3Texture(Renderer& renderer, const char* pathname) :
        _renderer(renderer),
        _texture(GL_INVALID_VALUE)
    {
        SDL_Surface* sdlSurface = IMG_Load(pathname);
        if (!sdlSurface)
            return;

        //  obtain compatible GL formats for this surface object
        GLTextureFormat glFormat;
        if (sdlToGLFormat(glFormat, sdlSurface->format->format))
        {
            GLsizei width = powerOf2((uint32_t)sdlSurface->w);
            GLsizei height = powerOf2((uint32_t)sdlSurface->h);

            glGenTextures(1, &_texture);
            glBindTexture(GL_TEXTURE_2D, _texture);
            glTexImage2D(GL_TEXTURE_2D, 0, glFormat.internalFormat, width, height, 0,
                            glFormat.format, glFormat.type,
                            sdlSurface->pixels);
            if (GLError("GL3Texture.glTexImage2D"))
            {
                glDeleteTextures(1, &_texture);
                _texture = GL_INVALID_VALUE;
            }
            else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
        }

        SDL_FreeSurface(sdlSurface);
    }

    GL3Texture::GL3Texture(Renderer& renderer, uint16_t w, uint16_t h,
            cinek_pixel_format format,
            const uint8_t* bytes, uint16_t stride) :
        _renderer(renderer)
    {
        //  Use a single channel texture 
        /*
        uint32_t sdlPixelFormat = SDL_PIXELFORMAT_UNKNOWN;
        //  certain input formats are not supported by SDL - convert to
        //  a workable format
        cinek_pixel_format sourcePixelFormat = kCinekPixelFormat_Unknown;
        switch (format)
        {
        case kCinekPixelFormat_A8:
            sourcePixelFormat = format;
            sdlPixelFormat = SDL_PIXELFORMAT_ARGB8888;
            break;
        default:
            sdlPixelFormat = convertToSDLPixelFormat(format);
            break;
        }
        if (sdlPixelFormat == SDL_PIXELFORMAT_UNKNOWN)
            return;

        _texture = SDL_CreateTexture(((SDLRenderer&)_renderer).getSDLRenderer(),
            sdlPixelFormat,
            SDL_TEXTUREACCESS_STATIC,
            w, h);
        if (!_texture)
            return;
        */
        
    }
    
    GL3Texture::~GL3Texture()
    {
        if (_texture != GL_INVALID_VALUE)
        {
            glDeleteTextures(1, &_texture);
            _texture = GL_INVALID_VALUE;
        }

    }
    
    GL3Texture::GL3Texture(GL3Texture&& other) :
        _renderer(other._renderer),
        _texture(other._texture)
    {
        other._texture = GL_INVALID_VALUE;
    }
        
    }   // namespace glx
}   // namespace cinekine