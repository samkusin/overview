//
//  BitmapLibrary.cpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "BitmapLibrary.hpp"
#include "BitmapAtlasLoader.hpp"
#include "RendererCLI.hpp"
#include "RenderDebug.hpp"
#include "Core/FileStreamBuf.hpp"


namespace cinekine {
    namespace glx {

    BitmapLibrary::BitmapLibrary(RendererCLI& renderer, const Allocator& allocator) :
        _allocator(allocator),
        _renderer(renderer),
        _atlasMap( std_allocator<AtlasMap::allocator_type >(_allocator) ),
        _nextAtlasHandle(0)
    {
    }

    BitmapLibrary::~BitmapLibrary() = default;

    cinek_bitmap_atlas BitmapLibrary::loadAtlas(const char* atlasName)
    {
        for (auto& atlas : _atlasMap)
        {
            if (!strcmp(atlas.second->getName(), atlasName))
                return atlas.first;
        }

        //  loads an atlas database.  using the unserializer, construct and build the
        //  atlas.  on success, add it to our atlas map.
        char path[MAX_PATH];
        snprintf(path, sizeof(path), "%s.json", atlasName);
        FileStreamBuf atlasFile(path);

        if (!atlasFile)
            return kCinekBitmapAtlas_Invalid;

        BitmapAtlasLoader atlasLoader;
        BitmapAtlas* atlas = nullptr;
        cinek_bitmap_atlas atlasHandle = kCinekBitmapAtlas_Invalid;
        RENDER_LOG_INFO("Loading atlas '%s'...", atlasName);

        atlasLoader.onImageLoadRequest([&atlas, &atlasHandle, &atlasName, &path, this](const char* textureName,
                                                                      uint16_t w, uint16_t h,
                                                                      cinek_pixel_format format,
                                                                      size_t bitmapCount) -> bool
            {
                if (atlas)
                {
                    RENDER_LOG_WARN("Ignoring image load request for '%s' since atlas was already created.", textureName);
                    return true;
                }
                snprintf(path, sizeof(path), "%s", textureName);
                std::shared_ptr<Texture> texture = _renderer.loadTexture(path);
                if (!(*texture))
                {
                    return false;
                }

                /**
                 * @todo (LOW) Use allocate_shared instead?
                 * Odd compile error when passing atlasName as a const char* (conversion to a
                 * const char* && occurs, which fails.)  This code isn't in a time-crticial
                 * section, so the dual allocation would not be an issue.
                 */
                atlas = _allocator.newItem<BitmapAtlas>(atlasName,
                                                        texture,
                                                        bitmapCount,
                                                        _allocator);
                std::shared_ptr<BitmapAtlas> atlasPtr(atlas,
                                                      SharedPtrDeleter<BitmapAtlas>(_allocator),
                                                      BitmapAtlasAllocator(_allocator));
                /*
                                    std::allocate_shared<BitmapAtlas, BitmapAtlasAllocator,
                                        const char* const,
                                        unique_ptr<Texture>&,
                                        size_t,
                                        const Allocator&>
                                    (
                                        BitmapAtlasAllocator(_renderer.getAllocator()),
                                        atlasName, texture, bitmapCount, _renderer.getAllocator()
                                    );
                */
                _atlasMap.emplace(_nextAtlasHandle, atlasPtr);
                //atlas = (*result.first).second.get();
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
                    RENDER_LOG_ERROR("Bitmap '%s' ignored.  Atlas hasn't been created.", bitmapName);
                    return;
                }
                RENDER_LOG_TRACE("Adding bitmap '%s' to atlas.", bitmapName);
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

    std::shared_ptr<BitmapAtlas> BitmapLibrary::getAtlas(cinek_bitmap_atlas handle) const
    {
        auto it = _atlasMap.find(handle);
        return it != _atlasMap.end() ? it->second : std::shared_ptr<BitmapAtlas>();
    }

    }   // namespace glx
}   // namespace cinekine