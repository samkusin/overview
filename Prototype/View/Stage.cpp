//
//  Stage.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved.
//  Author: Samir Sinha
//  License: The MIT License (MIT)
//

#include "View/Stage.hpp"
#include "Shared/GameTemplates.hpp"

#include "Engine/Debug.hpp"
#include "Engine/Model/TileLibrary.hpp"
#include "Engine/Model/SpriteLibrary.hpp"
#include "Engine/Model/TileGridMap.hpp"

namespace cinekine { namespace ovengine {

Stage::Stage(const GameTemplates& gameTemplates) :
    _tileLibrary(gameTemplates.tileLibrary()),
    _tileGridMap(*gameTemplates.tileGridMap())
{
}

const Tile& Stage::tileInfo(TileId tileId) const
{
    return _tileLibrary.tileFromCollectionAtIndex(slotFromTileId(tileId),
                                                  indexFromTileId(tileId));
}

void Stage::attachSpriteInstance(SpriteInstancePtr instance)
{
    // TODO: select the instance list based on the sprite's position
    _sprites.push_back(instance);
}

void Stage::detachSpriteInstance(SpriteInstancePtr instance)
{
    //  TODO: select the instance list based on the instance's position
    _sprites.remove(instance);
}

/*
 * Selects ViewModel (SpriteInstance reference) lists from the Stage that
 * intersect a supplied AABB.
 *
 * This method will select instance lists.  Each instance list has its own
 * AABB, and this method will choose one or more instance lists that
 * intersects with the given bounds.  The caller will have to cull
 * individual ViewModel (SpriteInstance) references from the returned lists
 * if it wants a precise result of all instances within the bounds.
 */
void Stage::selectInstanceLists(const AABB<Point>& bounds,
                                std::function<void(const SpriteInstanceList&)> cb) const
{
    auto overlayDims = _tileGridMap.overlayDimensions();
    AABB<Point> quadBounds(Point(0,0,0),
                           Point(overlayDims.x,
                                 overlayDims.y,
                                 _tileGridMap.overlayToFloorRatio()*2.f) /* z-coord HACK */);

    if (bounds.intersects(quadBounds) && cb)
    {
        cb(_sprites);
    }
}

} /* namespace ovengine */ } /* namespace cinekine */
