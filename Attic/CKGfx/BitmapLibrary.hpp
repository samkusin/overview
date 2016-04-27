//
//  BitmapLibrary.hpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_BitmapLibrary_hpp
#define CK_Graphics_BitmapLibrary_hpp

#include "BitmapAtlas.hpp"

#include <cinek/allocator.hpp>
#include <cinek/string.hpp>
#include <cinek/vector.hpp>

namespace cinek {
    namespace gfx {
    
    class TextureAtlas;

    //  A BitmapLibrary contains one or more BitmapAtlases
    //      Applications request atlas objects from a library.
    class BitmapLibrary
    {
        CK_CLASS_NON_COPYABLE(BitmapLibrary);

    public:
        BitmapLibrary(TextureAtlas& textureAtlas, const Allocator& allocator=Allocator());

        //  Returns a handle to an existing or newly loaded bitmap atlas.  Returns a null handle on failure.
        BitmapAtlasHandle loadAtlas(const char* atlasName);
        //  Unloads the specified atlas.
        void unloadAtlas(BitmapAtlasHandle handle);
        //  Returns an atlas given its handle, or null on failure.  This pointer will become invalid if
        //  unloadAtlas is called.
        const BitmapAtlas* atlas(BitmapAtlasHandle handle) const;

    private:
        TextureAtlas* _textureAtlas;
        Allocator _allocator;
        vector<std::pair<BitmapAtlasHandle, unique_ptr<BitmapAtlas>>> _atlases;
        vector<uint32_t> _freed;
        BitmapAtlasHandle _nextAtlasHandle;
    };

    }   // namespace gfx
}   // namespace cinek

#endif
