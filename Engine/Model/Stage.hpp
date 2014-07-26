///
/// @file
/// Class Definition for the Stage
///
/// @author     Samir Sinha
/// @date       05/20/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Model_Stage_hpp
#define Overview_Model_Stage_hpp

#include "Engine/Model/StageTypes.hpp"
#include "Engine/Model/TileGridMap.hpp"

#include "cinek/allocator.hpp"

namespace cinekine {
    namespace glx {
        class BitmapLibrary;
    }
    namespace ovengine {
        class TileLibrary;
        class SpriteLibrary;
    }
}

namespace cinekine { namespace ovengine {

class Stage
{
public:
    struct InitParameters
    {
        uint32_t overlayToFloorTileRatio = 4;
        uint16_t spriteLimit = 16;
    };

    Stage(const TileLibrary& tileDb,
          const SpriteLibrary& spriteDb,
          const MapBounds& bounds,
          const InitParameters& params,
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
     * Retrieve the TileGridMap
     * @return      A reference to a tile map.
     */
    TileGridMap& tileGridMap() {
        return _gridMap;
    }
    /**
     * Retrieve the const TileGridMap.
     * @return      A const reference to a tile map.
     */
    const TileGridMap& tileGridMap() const {
        return _gridMap;
    }

    /** @return The TileLibrary used by this Stage */
    const TileLibrary& tileLibrary() const {
        return _tileDb;
    }
    /** @return The SpriteLibrary used by this Stage */
    const SpriteLibrary& spriteDatabase() const {
        return _spriteDb;
    }
    /**
     * Shortcut method to retrieve Tile information from an ID using
     * the Stage's TileLibrary
     * @param  tileId The TileId
     * @return        Reference to the TileInfo
     */
    const Tile& tile(TileId tileId) const;

private:
    const TileLibrary& _tileDb;
    const SpriteLibrary& _spriteDb;
    MapBounds _bounds;
    TileGridMap _gridMap;

    vector<uint32_t> _freeSpriteInstanceIds;
};


} /* namespace ovengine */ } /* namespace cinekine */

#endif

