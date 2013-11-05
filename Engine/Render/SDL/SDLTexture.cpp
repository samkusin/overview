//
//  SDLTexture.cpp
//  Overview
//
//  Created by Samir Sinha on 9/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "SDLTexture.hpp"
// TODO - move into SDL/SDLRenderer.hpp
#include "../Renderer.hpp"

#include "Engine/Debug.hpp"

#include "SDL2/SDL_image.h"
#include "SDL2/SDL_pixels.h"

namespace cinekine {
    namespace ovengine {

    static uint32_t convertToSDLPixelFormat(cinek_pixel_format pixelFormat)
    {
        switch (pixelFormat)
        {
        case kCinekPixelFormat_RGBA8888:
            return SDL_PIXELFORMAT_ARGB8888;
        case kCinekPixelFormat_RGBA4444:
            return SDL_PIXELFORMAT_ARGB4444;
        case kCinekPixelFormat_RGB888:
            return SDL_PIXELFORMAT_RGB888;
        case kCinekPixelFormat_RGBA5551:
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
    static bool pixelsA8toFormat(uint16_t w, uint16_t h, const uint8_t* in, uint16_t stride,
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
            source += stride;
        }
        return true;
    }
      
    SDLTexture::SDLTexture(Renderer& renderer, const char* pathname) :
        _renderer(renderer),
        _texture(NULL)
    {
        _texture =  IMG_LoadTexture(_renderer.getSDLRenderer(), pathname);
    }

    SDLTexture::SDLTexture(Renderer& renderer, uint16_t w, uint16_t h,
            cinek_pixel_format format,
            const uint8_t* bytes, uint16_t stride) :
        _renderer(renderer),
        _texture(NULL)
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
        default:
            sdlPixelFormat = convertToSDLPixelFormat(format);
            break;
        }
        if (sdlPixelFormat == SDL_PIXELFORMAT_UNKNOWN)
            return;

        _texture = SDL_CreateTexture(_renderer.getSDLRenderer(),
            sdlPixelFormat,
            SDL_TEXTUREACCESS_STATIC,
            w, h);
        if (!_texture)
            return;
        
        int updateResult = -1;

        //  copy pixels to our texture.  for non-SDL formats, must create
        //  a temporary buffer to stuff our translated data to for upload
        //
        if (sourcePixelFormat != kCinekPixelFormat_Unknown)
        {
            SDL_PixelFormat* sdlPf = SDL_AllocFormat(sdlPixelFormat);
            if (sdlPf)
            {
                size_t bufferSize = sdlPf->BytesPerPixel * w * h;
                void *buffer = renderer.getAllocator().alloc(bufferSize);
                if (buffer)
                {
                    bool converted = false;
                    if (sourcePixelFormat == kCinekPixelFormat_A8)
                    {
                        //  alpha texture
                        converted = pixelsA8toFormat(w, h, bytes, stride, (uint8_t*)buffer, bufferSize, sdlPf);
                    }
                    if (converted)
                    {
                        updateResult = SDL_UpdateTexture(_texture, NULL, buffer, sdlPf->BytesPerPixel * w);
                    }
                    renderer.getAllocator().free(buffer);
                }
                SDL_FreeFormat(sdlPf);
            }
        }
        else
        {
            updateResult = SDL_UpdateTexture(_texture, NULL, bytes, stride);
        }
        
        if (updateResult < 0)
        {
            OVENGINE_LOG_ERROR("SDLTexture::SDLTexture - UpdateTexture failed: %s", SDL_GetError());
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
        
    }   // namespace ovengine
}   // namespace cinekine