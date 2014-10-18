///
/// @file
/// Contains the class definition for the TileGrid
///
/// @author     Samir Sinha
/// @date       07/20/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Model_TileGrid_hpp
#define Overview_Model_TileGrid_hpp

#include "Engine/Model/ModelTypes.hpp"
#include "Engine/Grid.hpp"

namespace cinekine { namespace ovengine {

/** Defines a grid of tiles */
using TileGrid = Grid<TileId>;
/** Defines a tilemap section */
using TileGridContainer = GridContainer<TileGrid>;

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

} /* namespace ovengine */ } /* namespace cinekine */

#endif
