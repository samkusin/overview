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
             const Allocator& allocator) :
    _tileDb(tileDb),
    _roomGraph(std::move(roomGraph)),
    _gridMap(std::move(tileGridMap))
{
}

const Tile& World::tileInfo(TileId tileId) const
{
    return _tileDb.tileFromCollectionAtIndex(slotFromTileId(tileId),
                                             indexFromTileId(tileId));
}

void World::attachSpriteInstance(SpriteInstancePtr instance)
{
    // TODO: select the instance list based on the sprite's position
    _sprites.push_back(instance);
}

void World::detachSpriteInstance(SpriteInstancePtr instance)
{
    //  TODO: select the instance list based on the instance's position
    _sprites.remove(instance);
}

/*
 * Selects ViewModel (SpriteInstance reference) lists from the World that
 * intersect a supplied AABB.
 *
 * This method will select instance lists.  Each instance list has its own
 * AABB, and this method will choose one or more instance lists that
 * intersects with the given bounds.  The caller will have to cull
 * individual ViewModel (SpriteInstance) references from the returned lists
 * if it wants a precise result of all instances within the bounds.
 */
void World::selectInstanceLists(const AABB<glm::ivec2>& bounds,
                                std::function<void(const SpriteInstanceList&)> cb)
{
    AABB<glm::ivec2> quadBounds(glm::ivec2(0,0), _gridMap.overlayDimensions());

    if (bounds.intersects(quadBounds) && cb)
    {
        cb(_sprites);
    }
}

} /* namespace ovengine */ } /* namespace cinekine */

