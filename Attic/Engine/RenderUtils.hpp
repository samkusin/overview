//
//  Render/Utils.hpp
//  Overview
//
//  Created by Samir Sinha on 9/18/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_Utils_hpp
#define CK_Graphics_Utils_hpp

#include <cstdint>

namespace cinek {
    namespace glx {

    // http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    //
    inline uint16_t powerOf2(uint16_t v) {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v++;
        return v;
    }

    inline uint32_t powerOf2(uint32_t v) {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        return v;
    }

    }
}

#endif
