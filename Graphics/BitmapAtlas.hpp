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
#include "RenderTypes.hpp"

#include "cinek/memorypool.hpp"
#include "cinek/string.hpp"
#include "cinek/vector.hpp"
#include "cinek/rendertypes.h"

#include <vector>

namespace cinekine {
    namespace glx {

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
        ~BitmapAtlas();

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
