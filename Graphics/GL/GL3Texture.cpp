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

namespace cinek {
    namespace glx {

    struct GLTextureFormat
    {
        GLint internalFormat;
        GLenum format;
        GLenum type;
        GLint byteAlign;
    };

    cinek_pixel_format sdlToCinekFormat(uint32_t sdlPixelFormat)
    {
        // With OpenGL, it's easier to upload 4-byte aligned images
        switch (sdlPixelFormat)
        {
        case SDL_PIXELFORMAT_ARGB8888:              // high order alpha, low order blue, 8-bit comp
            return kCinekPixelFormat_ARGB8888;
        case SDL_PIXELFORMAT_ARGB1555:              // high order alpha (1-bit), low order blue, 5-bit
            return kCinekPixelFormat_ARGB1555;
        default:
            break;
        }

        return kCinekPixelFormat_Unknown;
    }

    bool cinekToGLFormat(GLTextureFormat& glFormat, cinek_pixel_format ckFormat)
    {
        switch(ckFormat)
        {
        case kCinekPixelFormat_ARGB8888:
            glFormat.internalFormat = GL_RGBA8;
            glFormat.format = GL_BGRA;
            glFormat.type = GL_UNSIGNED_INT_8_8_8_8_REV;
            glFormat.byteAlign = 4;
            break;
        case kCinekPixelFormat_ABGR8888:
            glFormat.internalFormat = GL_RGBA8;
            glFormat.format = GL_RGBA;
            glFormat.type = GL_UNSIGNED_BYTE;
            glFormat.byteAlign = 4;
            break;
        case kCinekPixelFormat_ARGB4444:
            glFormat.internalFormat = GL_RGBA4;
            glFormat.format = GL_BGRA;
            glFormat.type = GL_UNSIGNED_SHORT_4_4_4_4_REV;
            glFormat.byteAlign = 2;
            break;
        case kCinekPixelFormat_RGB888:
            glFormat.internalFormat = GL_RGB8;
            glFormat.format = GL_BGR;
            glFormat.type = GL_UNSIGNED_BYTE;
            glFormat.byteAlign = 1;
            break;
        case kCinekPixelFormat_ARGB1555:
            glFormat.internalFormat = GL_RGB5_A1;
            glFormat.format = GL_BGRA;
            glFormat.type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
            glFormat.byteAlign = 2;
            break;
        case kCinekPixelFormat_RGB565:
            glFormat.internalFormat = GL_RGB8;
            glFormat.format = GL_BGRA;
            glFormat.type = GL_UNSIGNED_SHORT_5_6_5_REV;
            glFormat.byteAlign = 2;
            break;
        case kCinekPixelFormat_A8:
            glFormat.internalFormat = GL_RED;
            glFormat.format = GL_RED;
            glFormat.type = GL_UNSIGNED_BYTE;
            glFormat.byteAlign = 1;
            break;
        default:
            return false;
        }

        return true;
    };

    //////////////////////////////////////////////////////////////////////////

    GL3Texture::GL3Texture(const char* pathname) :
        _texture(0),
        _width(0),
        _height(0),
        _samplerFormat(kFormatNone)
    {
        SDL_Surface* sdlSurface = IMG_Load(pathname);
        if (!sdlSurface)
            return;

        cinek_pixel_format pf = sdlToCinekFormat(sdlSurface->format->format);

        _texture = createTexture(sdlSurface->w, sdlSurface->h, pf, (const uint8_t*)sdlSurface->pixels);

        SDL_FreeSurface(sdlSurface);
    }

    GL3Texture::GL3Texture(uint32_t w, uint32_t h,
            cinek_pixel_format format,
            const uint8_t* bytes) :
        _texture(0),
        _width(0),
        _height(0)
    {
        _texture = createTexture(w, h, format, bytes);
    }

    GLuint GL3Texture::createTexture(uint32_t w, uint32_t h,
                             cinek_pixel_format format,
                             const uint8_t* bytes)
    {
        //  obtain compatible GL formats for this surface object
        CK_ASSERT(!_texture && "Texture already exists - replacing, will likely leak GPU memory");
        GLTextureFormat glFormat;
        if (cinekToGLFormat(glFormat, format))
        {
            GLsizei width = powerOf2(w);
            GLsizei height = powerOf2(h);
            if (width != w || height != h)
            {
                /** @todo Support non power of two textures? */
                RENDER_LOG_DEBUG("GL3Texture.createTexture - dimensions are not power of 2 (%u,%u)", w, h);
                return 0;
            }

            glGenTextures(1, &_texture);
            glBindTexture(GL_TEXTURE_2D, _texture);
            glPixelStorei(GL_UNPACK_ALIGNMENT, glFormat.byteAlign);
            glTexImage2D(GL_TEXTURE_2D, 0, glFormat.internalFormat, width, height, 0,
                            glFormat.format, glFormat.type,
                            bytes);
            if (GLError("GL3Texture.createTexture - glTexImage2D"))
            {
                glBindTexture(GL_TEXTURE_2D, 0);
                glDeleteTextures(1, &_texture);
            }
            else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);
                _width = (uint32_t)width;
                _height = (uint32_t)height;
                _samplerFormat = format == kCinekPixelFormat_A8 ? kFormatRed : kFormatRGBA;
                return _texture;
            }
        }
        _texture = 0;
        return _texture;
    }

    GL3Texture::~GL3Texture()
    {
        if (_texture != 0)
        {
            glDeleteTextures(1, &_texture);
            _texture = 0;
        }

    }

    GL3Texture::GL3Texture(GL3Texture&& other) :
        _texture(other._texture)
    {
        other._texture = 0;
    }

    }   // namespace glx
}   // namespace cinek