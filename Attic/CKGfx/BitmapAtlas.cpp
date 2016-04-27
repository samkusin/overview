//
//  BitmapAtlas.cpp
//  Overview
//
//  Created by Samir Sinha on 8/11/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "BitmapAtlas.hpp"
#include <cstring>

namespace cinek {
    namespace gfx {

    //  Contains bitmaps mapped by name

    BitmapAtlas::BitmapAtlas(const char* name, TextureHandle texture,
                             size_t bitmapCount,
                             const Allocator& allocator) :
        _stringStack(bitmapCount*16 + 32, allocator),
        _name(""),
        _texture(texture),
        _bitmaps(allocator)
    {
        _bitmaps.reserve(bitmapCount);
        _name = _stringStack.create(name);
    }

    BitmapAtlas::BitmapAtlas(BitmapAtlas&& other) :
        _stringStack(std::move(other._stringStack)),
        _name(other._name),
        _texture(other._texture),
        _bitmaps(std::move(other._bitmaps))
    {
        _name = "";
        _texture = nullptr;
    }

    BitmapAtlas& BitmapAtlas::operator=(BitmapAtlas&& other)
    {
        _texture = other._texture;
        _name = other._name;
        _stringStack = std::move(other._stringStack);
        _bitmaps = std::move(other._bitmaps);
        
        other._texture = nullptr;
        other._name = "";
        
        return *this;
    }

    struct BitmapAtlasBitmap_Comparator
    {
        bool operator()(const BitmapInfo* bitmap, const char* name) const
        {
            return strcmp(bitmap->name, name) < 0;
        }
    };

    BitmapIndex BitmapAtlas::addBitmap(const BitmapInfo& info)
    {
        BitmapInfo* bitmap = nullptr;
        BitmapIndex h = bitmapIndexFromName(info.name);
        if (!h)
        {
            _bitmaps.emplace_back();
            bitmap = &_bitmaps.back();
            bitmap->name = _stringStack.create(info.name);
            h = (uint32_t)_bitmaps.size();
        }
        else
        {
            bitmap = &_bitmaps[h-1];
        }
        
        //  persist the name pointer (owned by the atlas) so we can perform
        //  a simple copy, then restore the name ptr accordingly
        auto name = bitmap->name;
        *bitmap = info;
        bitmap->name = name;

        return h;
    }

    BitmapIndex BitmapAtlas::bitmapIndexFromName(const char* name) const
    {
        for (auto it = _bitmaps.begin(); it != _bitmaps.end(); ++it)
        {
            if (!strcmp(it->name, name))
            {
                return (BitmapIndex)((it - _bitmaps.begin()) + 1);
            }
        }
        return kNullHandle;
    }

    const BitmapInfo* BitmapAtlas::bitmap(BitmapIndex h) const
    {
        if (!h || (h > _bitmaps.size()))
            return nullptr;
        
        return &_bitmaps[h-1];
    }

    }   // namespace gfx
}   // namespace cinek
