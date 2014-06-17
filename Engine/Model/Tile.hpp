//
//  Model/Tile.hpp
//  Overview
//
//  Created by Samir Sinha on 5/24/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Model_Tile_hpp
#define Overview_Model_Tile_hpp

#include "Engine/Model/ModelTypes.hpp"

namespace cinekine { namespace ovengine {

    struct Tile
    {
        cinek_bitmap bitmap = { kCinekBitmapAtlas_Invalid, kCinekBitmapIndex_Invalid };
        uint8_t categoryIndex = 0;
        uint8_t classIndex = 0;
        uint16_t flags = 0;
        AABB aabb;
    };

} /* namespace ovengine */ } /* namespace cinekine */

#endif
