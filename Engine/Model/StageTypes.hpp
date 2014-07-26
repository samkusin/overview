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

/** Defines a grid of tiles */
typedef Grid<TileId> TileGrid;
/** Defines a tilemap section */
typedef GridContainer<TileGrid> TileGridContainer;

    } /* overview */
} /* cinekine */

#endif
