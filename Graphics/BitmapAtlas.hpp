//
//  BitmapAtlas.hpp
//  Overview
//
//  Created by Samir Sinha on 8/11/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_BitmapAtlas_hpp
#define CK_Graphics_BitmapAtlas_hpp

#include "Texture.hpp"

#include "cinek/framework/memorypool.hpp"
#include "cinek/framework/string.hpp"
#include "cinek/framework/vector.hpp"
#include "cinek/rendermodel/types.h"

#include <vector>

namespace cinekine {
    namespace glx {
    
    //  Information used to render a bitmap.
    struct BitmapInfo
    {
        uint16_t x, y, w, h;
        uint16_t offX, offY;
        uint16_t srcW, srcH;
        string name;
        BitmapInfo(const Allocator& allocator=Allocator()): name(string_allocator(allocator)) {}
    };
    
    //  Contains bitmaps mapped by name to index
    //  Indices remain valid for the duration of the atlas object.  This is because
    //  the renderer will frequently reference a bitmap by index.  Lookups must be quick O(1).
    //  Name lookups can be slow since they should only occur during bitmap registration, which
    //  should not be an often occurrance.
    class BitmapAtlas
    {
        CK_CLASS_NON_COPYABLE(BitmapAtlas);

    public:
        //  The BitmapAtlas takes ownership of the Texture passed in.
        BitmapAtlas(const char* name, TexturePtr& texture,
                    size_t bitmapCount, const Allocator& allocator);
        BitmapAtlas(BitmapAtlas&& other);
        BitmapAtlas& operator=(BitmapAtlas&& other);
        ~BitmapAtlas() = default;
        
        const char* getName() const {
            return _name.c_str();
        }
        
        const Texture& getTexture() const {
            return *_texture;
        }

        //  use to build the atlas.
        cinek_bitmap_index addBitmap(BitmapInfo&& info);

        //  use to retrieve bitmap information.
        cinek_bitmap_index getBitmapIndex(const char* name) const;
        const BitmapInfo* getBitmapFromIndex(cinek_bitmap_index index) const;

    private:
        Allocator _allocator;
        string _name;
        TexturePtr _texture;
        ObjectPool<BitmapInfo> _bitmapPool;
        vector<BitmapInfo*> _bitmaps;
    };
    
    //  used for shared_ptr custom allocation
    typedef std_allocator<BitmapAtlas> BitmapAtlasAllocator;

    }   // namespace glx
}   // namespace cinekine

#endif
