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

inline TileId CompressTile(TileSlot slot, TileIndex index)
{
    return (slot << 12) + (index & 0xf000);
}

inline TileSlot SlotFromTileId(TileId id)
{
    return (id >> 12);
}

inline TileIndex IndexFromTileId(TileId id)
{
    return (id & 0x0fff);
}


Stage::Stage(const TileLibrary& tileDb,
             const SpriteLibrary& spriteDb,
             const MapBounds& bounds,
             const InitParameters& params,
             const Allocator& allocator) :
    _tileDb(tileDb),
    _spriteDb(spriteDb),
    _bounds(bounds),
    _gridMap(bounds.xUnits, bounds.yUnits, params.overlayToFloorTileRatio, allocator),
    _freeSpriteInstanceIds(std_allocator<uint32_t>(allocator))
{
    _freeSpriteInstanceIds.reserve(params.spriteLimit);
    _bounds.xUnits = _gridMap.overlay().columnCount();
    _bounds.yUnits = _gridMap.overlay().rowCount();
}

const Tile& Stage::tile(TileId tileId) const
{
    return _tileDb.tileFromCollectionAtIndex(SlotFromTileId(tileId),
                                             IndexFromTileId(tileId));
}


} /* namespace ovengine */ } /* namespace cinekine */

