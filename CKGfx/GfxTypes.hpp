//
//  GfxTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 4/5/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_GfxTypes_hpp
#define CK_Graphics_GfxTypes_hpp

#include <cinek/types.hpp>

namespace cinek {
    namespace gfx {

using TextureHandle = OffsetHandle;
using BitmapAtlasHandle = OffsetHandle;
using BitmapIndex = uint32_t;

struct BitmapHandle
{
    BitmapAtlasHandle atlas;
    BitmapIndex index = 0;
};


struct BitmapInfo
{
    uint16_t x, y, w, h;
    uint16_t offX, offY, offW, offH;
    uint16_t srcW, srcH;
    const char* name;
};


class TextureAtlas;
class ShaderLibrary;

    }   // namespace gfx
}   // namespace cinek

#endif
