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
