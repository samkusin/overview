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
#include "Engine/Model/StageSpriteInstance.hpp"

#include "cinek/allocator.hpp"
#include "cinek/string.hpp"
#include "cinek/memorypool.hpp"


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
    struct ResourceCounts
    {
        uint16_t spriteLimit = 16;
    };

    Stage(const TileLibrary& tileDb,
          const SpriteLibrary& spriteDb,
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
     * Retrieve the TileGrid at the specified Z layer.
     * @param z     A z-value in the range [zDown, zUp]
     * @return      A pointer to a tile map.
     */
    TileGrid* tileGridAtZ(int16_t z);
    /**
     * Retrieve the const TileGrid at the specified Z layer.
     * @param z     A z-value in the range [zDown, zUp]
     * @return      A const pointer to a tile map.
     */
    const TileGrid* tileGridAtZ(int16_t z) const;

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
    const TileLibrary& _tileDb;
    const SpriteLibrary& _spriteDb;
    MapBounds _bounds;
    vector<TileGrid> _tilemaps;

    vector<StageSpriteInstance> _spriteInstances;
    vector<uint32_t> _freeSpriteInstanceIds;
};


} /* namespace ovengine */ } /* namespace cinekine */

#endif

