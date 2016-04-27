//
//  BitmapAtlasLoader.hpp
//  Overview
//
//  Created by Samir Sinha on 8/18/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "BitmapAtlasLoader.hpp"

#include <cinek/debug.hpp>
#include <cstring>

namespace cinek {
    namespace gfx {

BitmapAtlasLoader::BitmapAtlasLoader
(
    const LoadTextureCb& loadTextureCb,
    const AtlasLoadedCb& atlasLoadedCb,
    const Allocator& allocator
) :
    _allocator(allocator),
    _loadTextureCb(loadTextureCb),
    _atlasLoadedCb(atlasLoadedCb)
{
}
   
bool BitmapAtlasLoader::startCollection
(
    const char* name,
    uint32_t modelCount
)
{
    TextureHandle texHandle = _loadTextureCb(_imgPath.c_str());
    if (!texHandle)
        return false;
    
    _atlas = allocate_unique<BitmapAtlas>
             (
                 _allocator,
                 name,
                 texHandle,
                 modelCount,
                 _allocator
             );
    
    return true;
}

bool BitmapAtlasLoader::parseAttribute
(
    const char* key,
    const JsonValue& value
)
{
    if (value.IsObject() && !strcmp(key, "meta"))
    {
        _imgPath = "";
        if (value.HasMember("image"))
        {
            _imgPath = value["image"].GetString();
        }
    }
    
    return true;
}

bool BitmapAtlasLoader::parseModel
(
    const char* key,
    JsonValue& object
)
{
    auto& frame = object;
    if (!frame.HasMember("filename") || !frame.HasMember("frame"))
    {
        CK_LOG_WARN("gfx",
                    "BitmapAtlasLoader - Invalid frame definition.  Skipping...");
        return false;
    }
    auto& frameRect = frame["frame"];
    auto& spriteSourceSize = frame["spriteSourceSize"];
    auto& sourceSize = frame["sourceSize"];
    
    BitmapInfo info;
    info.x = frameRect["x"].GetUint();
    info.y = frameRect["y"].GetUint();
    info.w = frameRect["w"].GetUint();
    info.h = frameRect["h"].GetUint();
    info.offX = spriteSourceSize["x"].GetUint();
    info.offY = spriteSourceSize["y"].GetUint();
    info.offW = spriteSourceSize["w"].GetUint();
    info.offH = spriteSourceSize["h"].GetUint();
    info.srcW = sourceSize["w"].GetUint();
    info.srcH = sourceSize["h"].GetUint();
    info.name = frame["filename"].GetString();
    
    return _atlas->addBitmap(info) != kNullHandle;
}

bool BitmapAtlasLoader::endCollection()
{
    _atlasLoadedCb(std::move(_atlas));
    return true;
}

    }   // namespace glx
}   // namespace cinek
