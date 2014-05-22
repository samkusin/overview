//
//  Stage.cpp
//  Overview
//
//  Created by Samir Sinha on 5/20/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Stage.hpp"
#include "SpriteDatabaseLoader.hpp"
#include "TileDatabaseLoader.hpp"

#include "Engine/Debug.hpp"
#include "Framework/FileStreamBuf.hpp"
#include "Graphics/BitmapLibrary.hpp"

namespace cinekine { namespace ovengine {

    Stage::Stage(glx::BitmapLibrary& bitmapLibrary,
                 const ResourceCounts& counts,
                 const cinek_ov_map_bounds& bounds,
                 const Allocator& allocator) :
        _allocator(allocator),
        _bitmapLibrary(bitmapLibrary),
        _tileDb(counts.tileLimit, _allocator),
        _spriteDb(counts.spriteLimit, _allocator),
        _map(bounds, _allocator)
    {

    }

    void Stage::loadTileDatabase(const char* filename)
    {
        FileStreamBuf dbStream(filename);
        if (!dbStream)
            return;

        TileDatabaseLoader tileDbLoader(_tileDb);
        tileDbLoader.unserialize(dbStream,
              [this](const char* atlasName) -> cinek_bitmap_atlas
              {
                  char path[MAX_PATH];
                  snprintf(path, sizeof(path), "textures/%s", atlasName);
                  return _bitmapLibrary.loadAtlas(path);
              },
              [this](cinek_bitmap_atlas atlas, const char* name) -> cinek_bitmap_index
              {
                  const glx::BitmapAtlas* bitmapAtlas = _bitmapLibrary.getAtlas(atlas).get();
                  if (!bitmapAtlas)
                      return kCinekBitmapIndex_Invalid;
                  return bitmapAtlas->getBitmapIndex(name);
              });
    }

    void Stage::loadSpriteDatabase(const char* filename)
    {
        FileStreamBuf dbStream(filename);
        if (!dbStream)
            return;

        SpriteDatabaseLoader spriteDbLoader(_spriteDb);
        spriteDbLoader.unserialize(dbStream,
            [this](const char* atlasName) -> cinek_bitmap_atlas
            {
                char path[MAX_PATH];
                snprintf(path, sizeof(path), "textures/%s", atlasName);
                return _bitmapLibrary.loadAtlas(path);
            },
            [this]( cinek_bitmap_atlas atlas, const char* name) -> cinek_bitmap_index
            {
                const glx::BitmapAtlas* bitmapAtlas = _bitmapLibrary.getAtlas(atlas).get();
                if (!bitmapAtlas)
                    return kCinekBitmapIndex_Invalid;
                return bitmapAtlas->getBitmapIndex(name);
            });
    }

} /* namespace ovengine */ } /* namespace cinekine */

