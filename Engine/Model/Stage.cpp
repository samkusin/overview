//
//  Stage.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved.
//  Author: Samir Sinha
//  License: The MIT License (MIT)
//

#include "Engine/Model/Stage.hpp"

#include "Engine/Debug.hpp"
#include "Engine/Model/TileLibrary.hpp"
#include "Engine/Model/SpriteLibrary.hpp"

namespace cinekine { namespace ovengine {

Stage::Stage(const TileLibrary& tileDb,
             const SpriteLibrary& spriteDb,
             const InitParameters& params,
             const Allocator& allocator) :
    _tileDb(tileDb),
    _spriteDb(spriteDb),
    _gridMap(params.floorDimX,
             params.floorDimY,
             params.overlayToFloorTileRatio,
             allocator),
    _spriteInstancePool(params.spriteLimit, allocator)
{
}


} /* namespace ovengine */ } /* namespace cinekine */

