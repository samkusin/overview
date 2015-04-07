//
//  BitmapLibrary.cpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "CKGfx/BitmapLibrary.hpp"
#include "CKGfx/BitmapAtlasLoader.hpp"
#include "CKGfx/TextureAtlas.hpp"

#include <cinek/filestreambuf.hpp>
#include <algorithm>

namespace cinek {
    namespace gfx {

BitmapLibrary::BitmapLibrary(TextureAtlas& textureAtlas, const Allocator& allocator) :
    _textureAtlas(&textureAtlas),
    _allocator(allocator),
    _atlases(allocator),
    _freed(allocator)
{
}

BitmapAtlasHandle BitmapLibrary::loadAtlas(const char* atlasName)
{
    auto atlasIt = std::find_if(_atlases.begin(), _atlases.end(),
        [atlasName](const decltype(_atlases)::value_type& r) -> bool {
            return !strcmp(r.second->name(), atlasName);
        });
    
    if (atlasIt != _atlases.end())
    {
        return atlasIt->first;
    }
    
    //  loads an atlas database.  using the unserializer, construct and build the
    //  atlas.  on success, add it to our atlas map.
    std::string path = atlasName;
    path+= ".json";
    FileStreamBuf atlasFile(path.c_str());

    if (!atlasFile)
        return nullptr;
    
    unique_ptr<BitmapAtlas> atlas;
    TextureAtlas* texAtlas = _textureAtlas;
    
    BitmapAtlasLoader atlasLoader(
        [&texAtlas](const char* texPath) -> TextureHandle
        {
            return texAtlas->loadTexture(texPath);
        },
        [&atlas](unique_ptr<BitmapAtlas>&& atlas)
        {
            atlas = std::move(atlas);
        },
        _allocator
    );
    
    if (!unserializeFromJSON(atlasLoader, atlasFile, atlasName))
        return nullptr;
    
    BitmapAtlasHandle atlasHandle = ++_nextAtlasHandle;
    if (_freed.empty())
    {
        atlasHandle.offs = (int)_atlases.size();
        _atlases.emplace_back();
    }
    else
    {
        atlasHandle.offs = _freed.back();
        _freed.pop_back();
    }
    
    _atlases[atlasHandle.offs].first = atlasHandle;
    _atlases[atlasHandle.offs].second = std::move(atlas);

    return atlasHandle;
}

void BitmapLibrary::unloadAtlas(BitmapAtlasHandle handle)
{
    if (!handle)
        return;
    
    auto& atlasRecord = _atlases[handle.offs];
    if (atlasRecord.first != handle)
        return;
    
    _atlases[handle.offs].second = nullptr;
    _atlases[handle.offs].first = nullptr;
    _freed.push_back(handle.offs);
}

const BitmapAtlas* BitmapLibrary::atlas(BitmapAtlasHandle handle) const
{
    if (!handle)
        return nullptr;
    if (handle.offs >= _atlases.size())
        return nullptr;
    
    auto& atlasRecord = _atlases[handle.offs];
    if (atlasRecord.first != handle)
        return nullptr;
        
    return atlasRecord.second.get();
}

    }   // namespace gfx
}   // namespace cinek