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

#include "cinek/rendermodel/types.h"
#include "cinek/cpp/ckmemorypool.hpp"
#include "cinek/cpp/ckstring.hpp"

#include <unordered_map>
#include <functional>

namespace cinekine {
    namespace glx {
    
    class Renderer;
    
    //  A BitmapLibrary contains one or more BitmapAtlases
    //      Applications request atlas objects from a library.
    class BitmapLibrary
    {
        CK_CLASS_NON_COPYABLE(BitmapLibrary);

    public:
        BitmapLibrary(Renderer& renderer);
        ~BitmapLibrary();
        
        //  Returns a handle to an existing or newly loaded bitmap atlas.  Returns a null handle on failure.
        cinek_bitmap_atlas loadAtlas(const char* atlasName);
        //  Unloads the specified atlas.
        void unloadAtlas(cinek_bitmap_atlas handle);
        //  Returns an atlas given its handle, or null on failure.  This pointer will become invalid if
        //  unloadAtlas is called.
        const BitmapAtlas* getAtlas(cinek_bitmap_atlas handle) const;

    private:
        Renderer& _renderer;
        typedef std::unordered_map<cinek_bitmap_atlas, BitmapAtlas,
                                    std::hash<cinek_bitmap_atlas>,
                                    std::equal_to<cinek_bitmap_atlas>,
                                    std_allocator<std::pair<cinek_bitmap_atlas, BitmapAtlas>>> AtlasMap;

        AtlasMap _atlasMap;
        cinek_bitmap_atlas _nextAtlasHandle;
    };
    
    }   // namespace glx
}   // namespace cinekine

#endif
