//
//  RenderTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 8/18/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_RenderTypes_hpp
#define Overview_RenderTypes_hpp

#include "cinek/rendermodel/types.h"
#include "cinek/cpp/ckstring.hpp"

namespace cinekine {
    namespace ovengine {
        
    //  Information used to render a bitmap.
    struct BitmapInfo
    {
        uint16_t x, y, w, h;
        uint16_t offX, offY;
        uint16_t srcW, srcH;
        string name;
        BitmapInfo(const Allocator& allocator=Allocator()): name(string_allocator(allocator)) {}
    };
        
    }   // namespace ovengine
}   // namespace cinekine

#endif
