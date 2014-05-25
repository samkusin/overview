//
//  Stage.cpp
//  Overview
//
//  Created by Samir Sinha on 5/20/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Stage.hpp"

#include "Engine/Debug.hpp"

namespace cinekine { namespace ovengine {

    Stage::Stage(const TileDatabase& tileDb,
                 const SpriteDatabase& spriteDb,
                 const ResourceCounts& counts,
                 const MapBounds& bounds,
                 const Allocator& allocator) :
        _tileDb(tileDb),
        _spriteDb(spriteDb),
        _bounds(bounds),
        _tilemaps(std_allocator<Tilemap>(allocator))
    {
        //  validation
        //
        if (!_bounds.xUnits || !_bounds.yUnits || !_bounds.zUnits)
        {
            OVENGINE_LOG_ERROR("Stage - Invalid bounds specified (%u x %u)", _bounds.xUnits, _bounds.yUnits);
            _bounds.xUnits = 1;
            _bounds.yUnits = 1;
            _bounds.zUnits = 1;
        }

        //  allocate tilemaps
        //      Tilemaps are not copyable - so can't initialize via initializer lists.
        _tilemaps.reserve(_bounds.zUnits);
        for (int32_t i = 0; i < _bounds.zUnits; ++i)
        {
            _tilemaps.push_back({ _bounds.xUnits, _bounds.yUnits, std_allocator<Tile>(allocator)});
        }
    }

    Tilemap* Stage::tilemapAtZ(int16_t z)
    {
        if (z < 0 || z >= _bounds.zUnits)
            return nullptr;
        return &_tilemaps[z];
    }

    const Tilemap* Stage::tilemapAtZ(int16_t z) const
    {
        if (z < 0 || z >= _bounds.zUnits)
            return nullptr;
        return &_tilemaps[z];
    }

} /* namespace ovengine */ } /* namespace cinekine */

