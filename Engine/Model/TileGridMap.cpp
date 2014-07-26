//
//  TileGridMap.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Model/TileGridMap.hpp"

namespace cinekine { namespace ovengine {

    TileGridMap::TileGridMap(uint32_t xDimension, uint32_t yDimension,
                             uint32_t overlayToFloorRatio,
                             const Allocator& allocator) :
        _floorGrid(yDimension, xDimension, allocator),
        _overlayGrid(yDimension*overlayToFloorRatio, xDimension*overlayToFloorRatio, allocator),
        _overlayToFloorRatio(overlayToFloorRatio)
    {
    }


} /* namespace ovengine */ } /* namespace cinekine */
