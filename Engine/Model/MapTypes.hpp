/**
 * \file    Engine/MapTypes.h
 *
 * Contains map specific types and type interfaces.
 *
 * \note    Created by Samir Sinha on 1/31/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_OVERVIEW_MAP_TYPES_H
#define CINEK_OVERVIEW_MAP_TYPES_H

#include "cinek/framework/types.hpp"
#include "cinek/rendermodel/types.h"

#include "glm/glm.hpp"

#ifdef __cplusplus
extern "C" {
#endif

/** Defines the XYZ bounds of a map. */
typedef struct cinek_ov_map_bounds_t
{
    uint16_t xUnits;    /**< Number of units/tiles on the x axis. */
    uint16_t yUnits;    /**< Number of units/tiles on the y axis. */
    uint16_t zUnits;
}
cinek_ov_map_bounds;

/** Defines a location on the map */
typedef glm::vec3 cinek_ov_pos;

/** Defines a boundary box (min/max) */
typedef struct cinek_ov_bounds {
    cinek_ov_pos min;
    cinek_ov_pos max;
}
cinek_ov_bounds;

/** Defines a tiled region within the tilemap. */
typedef struct cinek_tilemap_rect_t
{
    uint16_t left;      /**< The left bound of the tilemap rect. */
    uint16_t top;       /**< The top bound of the tilemap rect. */
    uint16_t right;     /**< The right bound of the tilemap rect. */
    uint16_t bottom;    /**< The bottom bound of the tilemap rect. */
}
cinek_tilemap_rect;

/**
 * @struct  cinek_map_tile
 * @brief   Represents a single tile
 */
typedef struct cinek_map_tile_t
{
    cinek_tile layer[2];
}
cinek_map_tile;

#ifdef __cplusplus
}
#endif

namespace cinekine { namespace ovengine {
    typedef cinek_map_tile Tile;
    typedef cinek_ov_map_bounds MapBounds;

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

} }

#endif /* CINEK_OVERVIEW_MAP_TYPES_H */
