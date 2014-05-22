/**
 * \file    Engine/Map.cpp
 *
 * Shared map data object containing terrain and prop objects.
 *
 * \note    Created by Samir Sinha on 3/2/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "Map.hpp"

#include "Engine/Debug.hpp"


namespace cinekine {
    namespace ovengine {

Map::Map(const cinek_ov_map_bounds& bounds, const Allocator& allocator) :
    _bounds(bounds),
    _tilemaps(std_allocator<Tilemap>(allocator))
{
    //  validation
    //
    if (!_bounds.xUnits || !_bounds.yUnits || !_bounds.zUnits)
    {
        OVENGINE_LOG_ERROR("Map - Invalid bounds specified (%u x %u)", _bounds.xUnits, _bounds.yUnits);
        _bounds.xUnits = 1;
        _bounds.yUnits = 1;
        _bounds.zUnits = 1;
    }

    //  allocate tilemaps
    //      Tilemaps are not copyable - so can't initialize via initializer lists.
    _tilemaps.reserve(_bounds.zUnits);
    for (int32_t i = 0; i < _bounds.zUnits; ++i)
    {
        _tilemaps.push_back({ _bounds.xUnits, _bounds.yUnits, std_allocator<cinek_map_tile>(allocator)});
    }
}


Map::~Map()
{
}


Tilemap* Map::tilemapAtZ(int16_t z)
{
    return const_cast<Tilemap*>(
            reinterpret_cast<const Map*>(this)->tilemapAtZ(z)
        );
}

const Tilemap* Map::tilemapAtZ(int16_t z) const
{
    if (z < 0 || z >= _bounds.zUnits)
        return nullptr;
    return &_tilemaps[z];
}

    } /* overview */
} /* cinekine */
