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

#include "cinek/allocator.hpp"
#include "cinek/vector.hpp"


namespace cinekine {
    namespace ovengine {

enum
{
    kTileDirection_N       = 0x0001,   /**< North */
    kTileDirection_NE      = 0x0002,   /**< Northeast */
    kTileDirection_E       = 0x0004,   /**< East */
    kTileDirection_SE      = 0x0008,   /**< Southeast */
    kTileDirection_S       = 0x0010,   /**< South */
    kTileDirection_SW      = 0x0020,   /**< Southwest */
    kTileDirection_W       = 0x0040,   /**< West */
    kTileDirection_NW      = 0x0080,   /**< Northwest */
    kTileDirection_Mask    = 0x00ff    /**< Bitmask defining direction */
};

enum
{
    kTileRole_Wall          = 0x0100,
    kTileRole_Corner        = 0x0200,
    kTileRole_Floor         = 0x1000
};

struct MapBounds
{
    uint16_t xUnits;    /**< Number of units/tiles on the x axis. */
    uint16_t yUnits;    /**< Number of units/tiles on the y axis. */
    uint16_t zUnits;
};

/** Defines a grid of tiles */
typedef Grid<Tile> Tilemap;
/** Defines a tilemap section */
typedef GridContainer<Tilemap> TilemapContainer;

    } /* overview */
} /* cinekine */

#endif
