//
//  BitmapAtlas.hpp
//  Overview
//
//  Created by Samir Sinha on 8/11/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Renderer_BitmapAtlas_hpp
#define Overview_Renderer_BitmapAtlas_hpp

#include "RenderTypes.hpp"

#include "cinek/cpp/ckmemorypool.hpp"
#include "cinek/cpp/ckstring.hpp"

#include "SDL_render.h"

#include <vector>

namespace cinekine {
    namespace ovengine {
    
    //  Contains bitmaps mapped by name to index
    //  Indices remain valid for the duration of the atlas object.  This is because
    //  the renderer will frequently reference a bitmap by index.  Lookups must be quick O(1).
    //  Name lookups can be slow since they should only occur during bitmap registration, which
    //  should not be an often occurrance.
    class BitmapAtlas
    {
        CK_CLASS_NON_COPYABLE(BitmapAtlas);

    public:
        //  The BitmapAtlas takes ownership of the SDL_Texture passed in.
        BitmapAtlas(const char* name, SDL_Texture* texture, size_t bitmapCount, const Allocator& allocator);
        BitmapAtlas(BitmapAtlas&& other);
        BitmapAtlas& operator=(BitmapAtlas&& other);
        ~BitmapAtlas();
        
        const char* getName() const {
            return _name.c_str();
        }

        //  use to build the atlas.
        cinek_bitmap_index addBitmap(BitmapInfo&& info);

        //  use to retrieve bitmap information.
        cinek_bitmap_index getBitmapIndex(const char* name) const;
        const BitmapInfo* getBitmapFromIndex(cinek_bitmap_index index, SDL_Texture** texture) const;
    
    private:
        Allocator _allocator;
        string _name;
        SDL_Texture* _texture;
        ObjectPool<BitmapInfo> _bitmapPool;
        std::vector<BitmapInfo*, std_allocator<BitmapInfo*>> _bitmaps;
    };
    
    }   // namespace ovengine
}   // namespace cinekine

#endif
