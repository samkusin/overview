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

#include "cinek/rendertypes.h"
#include "cinek/allocator.hpp"
#include "cinek/string.hpp"

#include <unordered_map>
#include <functional>

namespace cinek {
    namespace glx {

    class RendererCLI;

    //  A BitmapLibrary contains one or more BitmapAtlases
    //      Applications request atlas objects from a library.
    class BitmapLibrary
    {
        CK_CLASS_NON_COPYABLE(BitmapLibrary);

    public:
        BitmapLibrary(RendererCLI& renderer, const Allocator& allocator=Allocator());
        ~BitmapLibrary();

        //  Returns a handle to an existing or newly loaded bitmap atlas.  Returns a null handle on failure.
        cinek_bitmap_atlas loadAtlas(const char* atlasName);
        //  Unloads the specified atlas.
        void unloadAtlas(cinek_bitmap_atlas handle);
        //  Returns an atlas given its handle, or null on failure.  This pointer will become invalid if
        //  unloadAtlas is called.
        std::shared_ptr<BitmapAtlas> getAtlas(cinek_bitmap_atlas handle) const;

    private:
        Allocator _allocator;
        RendererCLI& _renderer;
        typedef std::unordered_map<cinek_bitmap_atlas,
                    std::shared_ptr<BitmapAtlas>,
                    std::hash<cinek_bitmap_atlas>,
                    std::equal_to<cinek_bitmap_atlas>,
                    std_allocator<std::pair<const cinek_bitmap_atlas,
                                            std::shared_ptr<BitmapAtlas>>>> AtlasMap;

        AtlasMap _atlasMap;
        cinek_bitmap_atlas _nextAtlasHandle;
    };

    }   // namespace glx
}   // namespace cinek

#endif
