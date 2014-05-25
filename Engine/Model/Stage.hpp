//
//  Stage.hpp
//  Overview
//
//  Created by Samir Sinha on 5/20/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Model_Stage_hpp
#define Overview_Model_Stage_hpp

#include "Engine/Model/StageTypes.hpp"

#include "cinek/allocator.hpp"


namespace cinekine {
    namespace glx {
        class BitmapLibrary;
    }
    namespace ovengine {
        class TileDatabase;
        class SpriteDatabase;
    }
}

namespace cinekine { namespace ovengine {

    class Stage
    {
    public:
        struct ResourceCounts
        {
            uint16_t spriteLimit = 16;
        };

        Stage(const TileDatabase& tileDb,
              const SpriteDatabase& spriteDb,
              const ResourceCounts& counts,
              const MapBounds& bounds,
              const Allocator& allocator);

        const TileDatabase& tileDatabase() const {
            return _tileDb;
        }
        const SpriteDatabase& spriteDatabase() const {
            return _spriteDb;
        }

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
        const TileDatabase& _tileDb;
        const SpriteDatabase& _spriteDb;
        MapBounds _bounds;
        vector<Tilemap> _tilemaps;
    };


} /* namespace ovengine */ } /* namespace cinekine */

#endif

