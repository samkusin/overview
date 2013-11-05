//
//  BitmapLibrary.cpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "BitmapLibrary.hpp"
#include "BitmapAtlasLoader.hpp"
#include "Renderer.hpp"
#include "Engine/Stream/FileStreamBuf.hpp"
#include "Debug.hpp"

#include "SDL2/SDL_image.h"
#include "SDL2/SDL_render.h"

#include "cinek/core/ckfspath.h"


namespace cinekine {
    namespace ovengine {
    
    BitmapLibrary::BitmapLibrary(Renderer& renderer) :
        _renderer(renderer),
        _atlasMap( std_allocator<AtlasMap::allocator_type >(_renderer.getAllocator()) ),
        _nextAtlasHandle(0)
    {
    }
    
    BitmapLibrary::~BitmapLibrary() = default;

    cinek_bitmap_atlas BitmapLibrary::loadAtlas(const char* atlasName)
    {
        for (auto& atlas : _atlasMap)
        {
            if (!strcmp(atlas.second.getName(), atlasName))
                return atlas.first;
        }

        //  loads an atlas database.  using the unserializer, construct and build the
        //  atlas.  on success, add it to our atlas map.
        char path[MAX_PATH];
        snprintf(path, sizeof(path), "bitmaps/%s.json", atlasName);
        FileStreamBuf atlasFile(path);
        
        if (!atlasFile)
            return kCinekBitmapAtlas_Invalid;
        
        BitmapAtlasLoader atlasLoader;
        BitmapAtlas* atlas = nullptr;
        cinek_bitmap_atlas atlasHandle = kCinekBitmapAtlas_Invalid;
        OVENGINE_LOG_INFO("Loading atlas '%s'...", atlasName);
        
        atlasLoader.onImageLoadRequest([&atlas, &atlasHandle, atlasName, &path, this](const char* textureName,
                                                                      uint16_t w, uint16_t h,
                                                                      cinek_pixel_format format,
                                                                      size_t bitmapCount) -> bool
            {
                if (atlas)
                {
                    OVENGINE_LOG_WARN("Ignoring image load request for '%s' since atlas was already created.", textureName);
                    return true;
                }
                snprintf(path, sizeof(path), "bitmaps/textures/%s", textureName);
                unique_ptr<Texture> texture = _renderer.loadTexture(path);
                if (!texture)
                {
                    return false;
                }
                BitmapAtlas tempAtlas(atlasName, texture, bitmapCount, _renderer.getAllocator());
                
                auto result = _atlasMap.emplace(_nextAtlasHandle, std::move(tempAtlas));
                atlas = &(*result.first).second;
                atlasHandle = _nextAtlasHandle;
                ++_nextAtlasHandle;
                return true;
            }
        );
        atlasLoader.onNewFrameRequest([&atlas, this](BitmapInfo& bitmapInfo)
            {
                const char* bitmapName = bitmapInfo.name.c_str();
                if (!atlas)
                {
                    OVENGINE_LOG_ERROR("Bitmap '%s' ignored.  Atlas hasn't been created.", bitmapName);
                    return;
                }
                OVENGINE_LOG_TRACE("Adding bitmap '%s' to atlas.", bitmapName);
                atlas->addBitmap(std::move(bitmapInfo));
            }
        );

        atlasLoader.unserialize(atlasFile);
        
        return atlasHandle;
    }

    void BitmapLibrary::unloadAtlas(cinek_bitmap_atlas handle)
    {
        if (handle != kCinekBitmapAtlas_Invalid)
        {
            _atlasMap.erase(handle);
        }
    }
    
    const BitmapAtlas* BitmapLibrary::getAtlas(cinek_bitmap_atlas handle) const
    {
        auto it = _atlasMap.find(handle);
        return it != _atlasMap.end() ? &it->second : nullptr;
    }

    }   // namespace ovengine
}   // namespace cinekine