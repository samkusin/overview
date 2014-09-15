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
#include "Engine/Model/SpriteInstance.hpp"
#include "Core/ObjectPool.hpp"

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

/**
 * @class   Stage
 * @brief   The data store for a simulation's model instances
 * @details The Stage is the highest level construct for a self-contained
 *          simulation world.   Its inputs consist of various 'model libraries'
 *          used to construct model instances.  For example, an application can
 *          create one or more stages sharing common Sprite or Tile model
 *          libraries.  Each Stage though contains its own set of model
 *          instances.
 */
class Stage
{
public:
    struct InitParameters
    {
        uint32_t overlayToFloorTileRatio = 4;
        uint16_t floorDimX = 12;
        uint16_t floorDimY = 12;
        uint16_t spriteLimit = 1024;
        uint16_t entityLimit = 256;
    };

    Stage(const TileLibrary& tileDb,
          const SpriteLibrary& spriteDb,
          const InitParameters& params,
          const Allocator& allocator);
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

private:
    const TileLibrary& _tileDb;
    const SpriteLibrary& _spriteDb;
    TileGridMap _gridMap;
    ObjectPool<SpriteInstance> _spriteInstancePool;
};


} /* namespace ovengine */ } /* namespace cinekine */

#endif

