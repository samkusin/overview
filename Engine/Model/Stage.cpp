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
             const MapBounds& bounds,
             const InitParameters& params,
             const Allocator& allocator) :
    _tileDb(tileDb),
    _spriteDb(spriteDb),
    _bounds(bounds),
    _gridMap(bounds.xUnits, bounds.yUnits, params.overlayToFloorTileRatio, allocator),
    _spriteInstances(allocator),
    _freeSpriteInstanceIds(allocator)
{
    _spriteInstances.reserve(params.spriteLimit);
    _freeSpriteInstanceIds.reserve(params.spriteLimit);
    _bounds.xUnits = _gridMap.overlay().columnCount();
    _bounds.yUnits = _gridMap.overlay().rowCount();
}


} /* namespace ovengine */ } /* namespace cinekine */

