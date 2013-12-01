//
//  SDLTexture.cpp
//  Overview
//
//  Created by Samir Sinha on 9/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "SDLTexture.hpp"
// TODO - move into SDL/SDLRenderer.hpp
#include "SDLRenderer.hpp"

#include "../RenderDebug.hpp"

#include "SDL2/SDL_image.h"
#include "SDL2/SDL_pixels.h"

namespace cinekine {
    namespace glx {

    static uint32_t convertToSDLPixelFormat(cinek_pixel_format pixelFormat)
    {
        switch (pixelFormat)
        {
        case kCinekPixelFormat_ARGB8888:
        case kCinekPixelFormat_ABGR8888:
            return SDL_PIXELFORMAT_ARGB8888;
        case kCinekPixelFormat_ARGB4444:
            return SDL_PIXELFORMAT_ARGB4444;
        case kCinekPixelFormat_RGB888:
            return SDL_PIXELFORMAT_RGB888;
        case kCinekPixelFormat_ARGB1555:
            return SDL_PIXELFORMAT_ARGB1555;
        case kCinekPixelFormat_RGB565:
            return SDL_PIXELFORMAT_RGB565;
            break;
        default:
            break;
        }
        return SDL_PIXELFORMAT_UNKNOWN;
    }
    
    //  convert this alpha-only format to the target SDL pixel format
    static bool pixelsA8toFormat(uint16_t w, uint16_t h, const uint8_t* in,
                                 uint8_t* out, size_t outSize, SDL_PixelFormat* outFormat)
    {
        //  validate our output buffer is the right size
        size_t requiredOutSize = outFormat->BytesPerPixel * (w * h);
        if (requiredOutSize > outSize)
            return false;
        
        const uint8_t* source = in;
        uint8_t* target = out;
        for (uint16_t row = 0; row < h; ++row)
        {
            for (uint16_t col = 0; col < w; ++col)
            {
                //  inefficient, but this method should only be called during texture upload
                uint32_t dest = SDL_MapRGBA(outFormat, 255, 255, 255, *(source+col));
                uint8_t ctr = outFormat->BytesPerPixel;
                while (ctr)
                {
                    *target = (uint8_t)dest;
                    ++target;
                    --ctr;
                    dest >>= 8;
                }
            }
            source += w;
        }
        return true;
    }

    //  convert this alpha-only format to the target SDL pixel format
    static bool pixelsABGR8888toFormat(uint16_t w, uint16_t h, const uint8_t* in,
                                       uint8_t* out, size_t outSize, SDL_PixelFormat* outFormat)
    {
        //  validate our output buffer is the right size
        size_t requiredOutSize = outFormat->BytesPerPixel * (w * h);
        if (requiredOutSize > outSize)
            return false;
        
        const uint8_t* source = in;
        uint8_t* target = out;
        for (uint16_t row = 0; row < h; ++row)
        {
            for (uint16_t col = 0; col < w; ++col)
            {
                //  inefficient, but this method should only be called during texture upload
                uint32_t dest = SDL_MapRGBA(outFormat, *source, *(source+1), *(source+2), *(source+3));
                uint8_t ctr = outFormat->BytesPerPixel;
                while (ctr)
                {
                    *target = (uint8_t)dest;
                    ++target;
                    --ctr;
                    dest >>= 8;
                }
                source += 4;
            }
        }
        return true;
    }
      
    SDLTexture::SDLTexture(Renderer& renderer, const char* pathname) :
        _renderer(renderer),
        _texture(NULL),
        _width(0),
        _height(0)
    {
        _texture =  IMG_LoadTexture(((SDLRenderer&)_renderer).getSDLRenderer(), pathname);
        if (_texture)
        {
            uint32_t format;
            int access, w, h;
            if (SDL_QueryTexture(_texture, &format, &access, &w, &h) < 0)
            {
                RENDER_LOG_ERROR("SDLTexture - %s", SDL_GetError());
                SDL_DestroyTexture(_texture);
                _texture = NULL;
            }
            else
            {
                _width = (uint32_t)w;
                _height = (uint32_t)h;
            }
        }
    }

    SDLTexture::SDLTexture(Renderer& renderer, uint32_t w, uint32_t h,
            cinek_pixel_format format,
            const uint8_t* bytes) :
        _renderer(renderer),
        _texture(NULL),
        _width(w),
        _height(h)
    {
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
        case kCinekPixelFormat_ABGR8888:
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
        if (!bytes)
            return;
        
        int updateResult = -1;

        //  copy pixels to our texture.  for non-SDL formats, must create
        //  a temporary buffer to stuff our translated data to for upload
        //
        SDL_PixelFormat* sdlPf = SDL_AllocFormat(sdlPixelFormat);
        if (sdlPf)
        {
            if (sourcePixelFormat != kCinekPixelFormat_Unknown)
            {
                size_t bufferSize = sdlPf->BytesPerPixel * w * h;
                void *buffer = renderer.getAllocator().alloc(bufferSize);
                if (buffer)
                {
                    bool doUpdate = false;
                    switch (sourcePixelFormat)
                    {
                    case kCinekPixelFormat_A8:
                        doUpdate = pixelsA8toFormat(w, h, bytes, (uint8_t*)buffer, bufferSize, sdlPf);
                        break;
                    case kCinekPixelFormat_ABGR8888:
                        doUpdate = pixelsABGR8888toFormat(w, h, bytes, (uint8_t*)buffer, bufferSize, sdlPf);
                        break;
                    default:
                        break;
                    }
                    if (doUpdate)
                    {
                        updateResult = SDL_UpdateTexture(_texture, NULL, buffer, sdlPf->BytesPerPixel * w);
                    }
                    renderer.getAllocator().free(buffer);
                }
            }
            else
            {
                updateResult = SDL_UpdateTexture(_texture, NULL, bytes, sdlPf->BytesPerPixel * w);
            }
            SDL_FreeFormat(sdlPf);
        }
        
        if (updateResult < 0)
        {
            RENDER_LOG_ERROR("SDLTexture::SDLTexture - UpdateTexture failed: %s", SDL_GetError());
            SDL_DestroyTexture(_texture);
            _texture = NULL;
        }
    }
    
    SDLTexture::~SDLTexture()
    {
        if (_texture)
        {
            SDL_DestroyTexture(_texture);
        }
    }
    
    SDLTexture::SDLTexture(SDLTexture&& other) :
        _renderer(other._renderer),
        _texture(std::move(other._texture))
    {
        other._texture = NULL;
    }


    }   // namespace glx
}   // namespace cinekine