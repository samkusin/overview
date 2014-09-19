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

#include "Engine/Model/WorldTypes.hpp"
#include "Engine/Model/RoomGraph.hpp"
#include "Engine/Model/TileGridMap.hpp"
#include "Engine/Model/Tile.hpp"
#include "Engine/Model/SpriteInstance.hpp"

#include "cinek/intrusive_list.hpp"
#include "cinek/objectpool.hpp"
#include "cinek/string.hpp"

#include <functional>

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
          const Allocator& allocator);

    /** @return A const reference to a tile map. */
    const TileGridMap& tileGridMap() const {
        return _gridMap;
    }
    /** @return A reference to the tile map. */
    TileGridMap& tileGridMap() {
        return _gridMap;
    }
    /**
     * @param  tileId A TileGridMap tile identifier
     * @return A TileInfo mapped to the supplied TileId
     */
    const Tile& tileInfo(TileId tileId) const;
    /**
     * Attaches a SpriteInstance to the World
     *
     * @param  instance The instance to attach from the World
     */
    void attachSpriteInstance(SpriteInstancePtr instance);
    /**
     * Detaches the SpriteInstance from the Wrold
     *
     * @param  instance The instance to detach from the World
     */
    void detachSpriteInstance(SpriteInstancePtr instance);

    /**
     * Selects ViewModel (SpriteInstance reference) lists from the World that
     * intersect a supplied AABB.
     *
     * This method will select instance lists.  Each instance list has its own
     * AABB, and this method will choose one or more instance lists that
     * intersects with the given bounds.  The caller will have to cull
     * individual ViewModel (SpriteInstance) references from the returned lists
     * if it wants a precise result of all instances within the bounds.
     *
     * @param bounds A bounding box that intersects one or more World sections.
     *               Each section has its own AABB and instance list.  See the
     *               method definition for details on this process.
     * @param cb     Callback issued for each selected SpriteInstanceList
     */
    void selectInstanceLists(const AABB<glm::ivec2>& bounds,
                             std::function<void(const SpriteInstanceList&)> cb);

private:
    const TileLibrary& _tileDb;
    RoomGraph _roomGraph;
    TileGridMap _gridMap;

    SpriteInstanceList _sprites;
};


} /* namespace ovengine */ } /* namespace cinekine */

#endif

