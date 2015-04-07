//
//  BitmapAtlas.hpp
//  Overview
//
//  Created by Samir Sinha on 8/11/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_BitmapAtlas_hpp
#define CK_Graphics_BitmapAtlas_hpp

#include "GfxTypes.hpp"

#include <cinek/cstringstack.hpp>
#include <cinek/vector.hpp>

namespace cinek {
    namespace gfx {

    //  Contains bitmaps mapped by name to index
    //  Indices remain valid for the duration of the atlas object.  This is because
    //  the renderer will frequently reference a bitmap by index.  Lookups must be quick O(1).
    //  Name lookups can be slow since they should only occur during bitmap registration, which
    //  should not be an often occurrance.
    class BitmapAtlas
    {
        CK_CLASS_NON_COPYABLE(BitmapAtlas);

    public:
        BitmapAtlas
        (
            const char* name,
            TextureHandle texHandle,
            size_t bitmapCount,
            const Allocator& allocator
        );
        BitmapAtlas(BitmapAtlas&& other);
        BitmapAtlas& operator=(BitmapAtlas&& other);
        
        const char* name() const {
            return _name;
        }
        TextureHandle textureHandle() const {
            return _texture;
        }
        
        //  use to build the atlas.
        BitmapIndex addBitmap(const BitmapInfo& info);

        //  use to retrieve bitmap information.
        BitmapIndex bitmapIndexFromName(const char* name) const;
        const BitmapInfo* bitmap(BitmapIndex handle) const;

    private:
        CStringStack _stringStack;
        const char* _name;
        TextureHandle _texture;
        vector<BitmapInfo> _bitmaps;
    };

    }   // namespace gfx
}   // namespace cinek

#endif
