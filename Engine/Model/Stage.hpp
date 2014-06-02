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
#include "Engine/Model/StageSpriteInstance.hpp"

#include "cinek/allocator.hpp"
#include "cinek/string.hpp"
#include "cinek/memorypool.hpp"


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

        /** @return The TileDatabase used by this Stage */
        const TileDatabase& tileDatabase() const {
            return _tileDb;
        }
        /** @return The SpriteDatabase used by this Stage */
        const SpriteDatabase& spriteDatabase() const {
            return _spriteDb;
        }
        /**
         * Shortcut method to retrieve Tile information from an ID using
         * the Stage's TileDatabase
         * @param  tileId The TileId
         * @return        Reference to the TileInfo
         */
        const TileInfo& tileInfo(TileId tileId) const;

        /**
         * Constructs a SpriteInstance from the given template and its initial
         * position on the stage (world)
         *
         * @param  name  The sprite template name to use
         * @param  pos   The initial sprite position
         * @return A pair containing the sprite instance handle and pointer.
         *         The pointer should not be retained by other objects - it is
         *         meant for immediate access only
         */
        std::pair<SpriteInstanceId, StageSpriteInstance*>
                createSpriteInstance(const string& name,
                                     const glm::vec3& pos);
        /**
         * Returns a pointer given the sprite instance's handle
         * @param  instanceHandle The instance handle
         * @return A pointer to the instance
         */
        StageSpriteInstance* spriteInstance(SpriteInstanceId instanceHandle);
        /**
         * Releases a SpriteInstance pointed to by the given handle
         * @param instanceHandle A sprite instance handle
         */
        void releaseSpriteInstance(SpriteInstanceId instanceHandle);

    private:
        const TileDatabase& _tileDb;
        const SpriteDatabase& _spriteDb;
        MapBounds _bounds;
        vector<Tilemap> _tilemaps;

        vector<StageSpriteInstance> _spriteInstances;
        vector<uint32_t> _freeSpriteInstanceIds;
    };


} /* namespace ovengine */ } /* namespace cinekine */

#endif

