//
//  TextureAtlas.cpp
//  Overview
//
//  Created by Samir Sinha on 4/5/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "TextureLibrary.hpp"

#include <cinek/debug.hpp>
#include <algorithm>

namespace cinek {
    namespace gfx {
    
    
struct TextureLibrary::Record
{
    Texture texture;
    TextureLibrary* owner;
    int refcount;
};

TextureLibrary::TextureLibrary(const InitParams& params) :
    _textureNames(params.stringStackSize),
    _recordsPool(params.textureCount)
{
    _orderedRecords.reserve(params.textureCount);
}

TextureLibrary::TextureLibrary(TextureLibrary&& other) :
    _orderedRecords(std::move(other._orderedRecords)),
    _textureNames(std::move(other._textureNames)),
    _recordsPool(std::move(other._recordsPool))
{
}

TextureLibrary& TextureLibrary::operator=(TextureLibrary&& other)
{
    _orderedRecords = std::move(other._orderedRecords);
    _textureNames = std::move(other._textureNames);
    _recordsPool = std::move(other._recordsPool);
    
    return *this;
}
        
void TextureLibrary::add(const char* name, Texture&& texture)
{
}

void TextureLibrary::remove(const char* name)
{
}
        
TextureHandle TextureLibrary::find(const char* name)
{
}

void TextureLibrary::releaseRecord(TextureLibrary::Record *record)
{
    //  Our TextureLibrary manages the ordered records list which keeps
    //  a reference to the Record.  When removing a Texture from the library
    //  that method decrements the refcount and explicitly will call this method
    //  upon reaching zero.  Likewise our ManagedHandles will call this method
    //  when refcount reaches zero.
    _recordsPool.destruct(record);
}

        
    }   // namespace gfx
    
template<> void ManagedHandle<gfx::Texture, gfx::TextureLibrary>::acquire()
{
    auto record = reinterpret_cast<gfx::TextureLibrary::Record*>(_resource);
    ++record->refcount;
    CK_ASSERT(record->refcount > 0);
}

template<> void ManagedHandle<gfx::Texture, gfx::TextureLibrary>::release()
{
    auto record = reinterpret_cast<gfx::TextureLibrary::Record*>(_resource);
    CK_ASSERT(record->refcount > 0);
    --record->refcount;
    if (!record->refcount) {
        record->owner->releaseRecord(record);
    }
}

}   // namespace cinek
