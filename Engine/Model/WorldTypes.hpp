/**
 * \file    Model/StageTypes.hpp
 *
 * Shared map data object containing terrain and prop objects.
 *
 * \note    Created by Samir Sinha on 3/2/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef Overview_Model_StageTypes_hpp
#define Overview_Model_StageTypes_hpp

#include "Engine/Model/ModelTypes.hpp"
#include "Engine/Grid.hpp"

namespace cinekine {
    namespace ovengine {
        class SpriteInstance;
    }
}


namespace cinekine {
    namespace ovengine {

/** Defines a grid of tiles */
using TileGrid = Grid<TileId>;
/** Defines a tilemap section */
using TileGridContainer = GridContainer<TileGrid>;

using SpriteInstancePtr = SpriteInstance*;
using ConstSpriteInstancePtr = const SpriteInstance*;

inline TileId compressTileToId(TileSlot slot, TileIndex index)
{
    return (slot << 12) + (index & 0x0fff);
}

inline TileSlot slotFromTileId(TileId id)
{
    return (id >> 12);
}

inline TileIndex indexFromTileId(TileId id)
{
    return (id & 0x0fff);
}

    } /* overview */
} /* cinekine */

#endif
