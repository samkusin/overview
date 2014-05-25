/**
 * \file    Model/Map.hpp
 *
 * Shared map data object containing terrain and prop objects.
 *
 * \note    Created by Samir Sinha on 3/2/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef Overview_Model_Map_hpp
#define Overview_Model_Map_hpp

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

/**
 * @class Map map.hpp "cinek/overview/map.hpp"
 * @brief Contains map data for display and interaction by a game simulation.
 *
 * A map is comprised of a terrain grid of tiles, and a quad-tree of props.
 *
 * Ownership rules:
 * - Map owns its QuadTree
 * - Map owns
 */
class Map
{
    CK_CLASS_NON_COPYABLE(Map);

public:
    /**
     *  Constructor that creates an empty map with specified dimensions.
     *
     *  @param  bounds  Map x,y and z bounds.
     */
    Map(const MapBounds& bounds, const Allocator& allocator = Allocator());
    /**
     * Destructor.
     */
    ~Map();

    /**
     * Retrieve map coordinate bounds.
     * Can be used to calculate an index into a tile array.
     * @return  Reference to the map bounds structure.
     */
    const MapBounds& bounds() const {
        return _bounds;
    }

    /**
     * Retrieve the tilemap at the specified Z layer.
     * @param z     A z-value in the range [zDown, zUp]
     * @return      A pointer to a tile map.
     */
    Tilemap* tilemapAtZ(int16_t z);
    /**
     * Retrieve the const tilemap at the specified Z layer.
     * @param z     A z-value in the range [zDown, zUp]
     * @return      A const pointer to a tile map.
     */
    const Tilemap* tilemapAtZ(int16_t z) const;

private:
    //  grid xyz
    MapBounds _bounds;
    vector<Tilemap> _tilemaps;
};

/**
 * Allocator used for shared Map objects.  Maps are typically shared amongst
 * objects in the Overview framework.
 */
typedef std_allocator<Map> MapAllocator;

    } /* overview */
} /* cinekine */

#endif
