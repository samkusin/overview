/**
 * \file    Engine/Map.hpp
 *
 * Shared map data object containing terrain and prop objects.
 *
 * \note    Created by Samir Sinha on 3/2/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef Overview_Map_hpp
#define Overview_Map_hpp

#include "MapTypes.hpp"
#include "Engine/Grid.hpp"

#include "cinek/framework/allocator.hpp"
#include "cinek/framework/vector.hpp"


namespace cinekine {
    namespace ovengine {

typedef cinek_map_tile Tile;

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
    Map(const cinek_ov_map_bounds& bounds, const Allocator& allocator = Allocator());
    /**
     * Destructor.
     */
    ~Map();

    /**
     * Retrieve map coordinate bounds.
     * Can be used to calculate an index into a tile array.
     * @return  Reference to the map bounds structure.
     */
    const cinek_ov_map_bounds& bounds() const {
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
    cinek_ov_map_bounds _bounds;
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
