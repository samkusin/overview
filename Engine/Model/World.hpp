///
/// @file
/// Class Definition for the World
///
/// @author     Samir Sinha
/// @date       05/20/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Model_World_hpp
#define Overview_Model_World_hpp

#include "Engine/Model/StageTypes.hpp"
#include "Engine/Model/RoomGraph.hpp"
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
 * @class   World
 * @brief   The data store for a simulation's model instances
 * @details The World is the highest level construct for a self-contained
 *          simulation world.   Its inputs consist of various 'model libraries'
 *          used to construct model instances.  For example, an application can
 *          create one or more worlds sharing common Sprite or Tile model
 *          libraries.  Each World though contains its own set of model
 *          instances.
 */
class World
{
public:
    World(RoomGraph&& roomGraph,
          TileGridMap&& tileGridMap,
          const TileLibrary& tileDb,
          const SpriteLibrary& spriteDb,
          const Allocator& allocator);

    /** @return      A const reference to a tile map. */
    const TileGridMap& tileGridMap() const {
        return _gridMap;
    }
    /**
     * @param  tileId A TileGridMap tile identifier
     * @return        A TileInfo mapped to the supplied TileId
     */
    const Tile& tileInfo(TileId tileId) const;

private:
    const TileLibrary& _tileDb;
    const SpriteLibrary& _spriteDb;
    RoomGraph _roomGraph;
    TileGridMap _gridMap;
};


} /* namespace ovengine */ } /* namespace cinekine */

#endif

