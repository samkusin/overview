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
#include "Engine/Model/Tile.hpp"
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

const uint32_t kTileDirections_North = kTileDirection_NW +
                                       kTileDirection_N +
                                       kTileDirection_NE;
const uint32_t kTileDirections_South = kTileDirection_SW +
                                       kTileDirection_S +
                                       kTileDirection_SE;
const uint32_t kTileDirections_East  = kTileDirection_NE +
                                       kTileDirection_E +
                                       kTileDirection_SE;
const uint32_t kTileDirections_West  = kTileDirection_NW +
                                       kTileDirection_W +
                                       kTileDirection_SW;

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

struct TileInstance
{
    TileId layer[2];
    //  circular linked list of sprites, 0 if there are none
    SpriteInstanceId firstSpriteInstanceId;
    TileInstance(): layer {}, firstSpriteInstanceId(0) {}
    TileInstance(const TileInstance& tile) :
        layer { tile.layer[0], tile.layer[1] },
        firstSpriteInstanceId( tile.firstSpriteInstanceId)
    {
    }
    TileInstance& operator=(const TileInstance& tile)
    {
        layer[0] = tile.layer[0];
        layer[1] = tile.layer[1];
        firstSpriteInstanceId = tile.firstSpriteInstanceId;
        return *this;
    }
    TileInstance(TileInstance&& tile):
        layer { tile.layer[0], tile.layer[1] },
        firstSpriteInstanceId(tile.firstSpriteInstanceId)
    {
        tile.layer[0] = 0;
        tile.layer[1] = 0;
        tile.firstSpriteInstanceId = 0;
    }
};

/** Defines a grid of tiles */
typedef Grid<TileInstance> TileGrid;
/** Defines a tilemap section */
typedef GridContainer<TileGrid> TileGridContainer;

    } /* overview */
} /* cinekine */

#endif
