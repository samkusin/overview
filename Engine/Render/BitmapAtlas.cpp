//
//  BitmapAtlas.cpp
//  Overview
//
//  Created by Samir Sinha on 8/11/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "BitmapAtlas.hpp"
#include "cinek/rendermodel/types.h"
#include <cstring>

namespace cinekine {
    namespace ovengine {

        //  Contains bitmaps mapped by name
        
    BitmapAtlas::BitmapAtlas(const char* name, SDL_Texture* texture,
                             size_t bitmapCount,
                             const Allocator& allocator) :
        _allocator(allocator),
        _name(name, string_allocator(_allocator)),
        _texture(texture),
        _bitmapPool(bitmapCount, _allocator),
        _bitmaps(std_allocator<BitmapInfo*>(_allocator))
    {
        _bitmaps.reserve(bitmapCount);
    }
    
    BitmapAtlas::BitmapAtlas(BitmapAtlas&& other) :
        _texture(other._texture),
        _bitmapPool(std::move(other._bitmapPool)),
        _bitmaps(std::move(other._bitmaps))
    {
        other._texture = nullptr;
    }
    
    BitmapAtlas& BitmapAtlas::operator=(BitmapAtlas&& other)
    {
        _texture = other._texture;
        _bitmapPool = std::move(other._bitmapPool);
        _bitmaps = std::move(other._bitmaps);
        other._texture = nullptr;
        return *this;
    }
    
    BitmapAtlas::~BitmapAtlas()
    {
        if (_texture)
        {
            SDL_DestroyTexture(_texture);
        }
    }
    
    struct BitmapAtlasBitmap_Comparator
    {
        bool operator()(const BitmapInfo* bitmap, const string& name) const
        {
            return bitmap->name < name;
        }
    };
    
    cinek_bitmap_index BitmapAtlas::addBitmap(BitmapInfo&& info)
    {
        cinek_bitmap_index index = getBitmapIndex(info.name.c_str());
       
        BitmapInfo* bitmap = nullptr;
        if (index == kCinekBitmapIndex_Invalid)
        {
            bitmap = _bitmapPool.allocateAndConstruct(_allocator);
            _bitmaps.push_back(bitmap);
            index = _bitmaps.size()-1;
        }
        else
        {
            bitmap = _bitmaps[index];
        }
        *bitmap = std::move(info);

        return index;
    }
            
    cinek_bitmap_index BitmapAtlas::getBitmapIndex(const char* name) const
    {
        cinek_bitmap_index index = 0;
        for (; index < _bitmaps.size(); ++index)
        {
            if (_bitmaps[index]->name.compare(name) == 0)
                return index;
        }
        return kCinekBitmapIndex_Invalid;
    }
    
    const BitmapInfo* BitmapAtlas::getBitmapFromIndex(cinek_bitmap_index index,
                                                      SDL_Texture** texture) const
    {
        if (index >= _bitmaps.size())
            return nullptr;
        *texture = _texture;
        return _bitmaps[index];
    }
        
    }   // namespace ovengine
}   // namespace cinekine
