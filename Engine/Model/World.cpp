//
//  World.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved.
//  Author: Samir Sinha
//  License: The MIT License (MIT)
//

#include "Engine/Model/World.hpp"

#include "Engine/Debug.hpp"
#include "Engine/Model/TileLibrary.hpp"
#include "Engine/Model/SpriteLibrary.hpp"

namespace cinekine { namespace ovengine {

World::World(RoomGraph&& roomGraph,
             TileGridMap&& tileGridMap,
             const TileLibrary& tileDb,
             const SpriteLibrary& spriteDb,
             const Allocator& allocator) :
    _tileDb(tileDb),
    _spriteDb(spriteDb),
    _roomGraph(std::move(roomGraph)),
    _gridMap(std::move(tileGridMap))
{
}

const Tile& World::tileInfo(TileId tileId) const
{
    return _tileDb.tileFromCollectionAtIndex(slotFromTileId(tileId),
                                             indexFromTileId(tileId));
}


} /* namespace ovengine */ } /* namespace cinekine */

