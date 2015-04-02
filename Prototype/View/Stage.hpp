///
/// @file
/// Class Definition for the World
///
/// @author     Samir Sinha
/// @date       05/20/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Model_Stage_hpp
#define Overview_Model_Stage_hpp

#include "View/StageTypes.hpp"

#include "Engine/Model/Tile.hpp"
#include "Engine/Model/SpriteInstance.hpp"

#include "cinek/intrusive_list.hpp"
#include "cinek/objectpool.hpp"
#include "cinek/string.hpp"

#include <functional>

namespace cinek {
    namespace overview {
        class GameTemplates;
        class TileLibrary;
        class TileGridMap;
        class StaticWorldMap;
    }
}

namespace cinek { namespace overview {

/**
 * @class   Stage
 * @brief   The Stage store for a simulation's model instances
 * @details The World is the highest level construct for a self-contained
 *          container of viewable objets.   Its inputs consist of various
 *          'model libraries' used to construct model instances.  For example,
 *          an application can create one or more worlds sharing common Sprite
 *          or Tile model libraries.
 */
class Stage
{
    CK_CLASS_NON_COPYABLE(Stage);
    
public:
    Stage(const GameTemplates& gameTemplates,
          const StaticWorldMap& staticWorldMap);

    /** @return A const reference to a tile map. */
    const TileGridMap& tileGridMap() const;
    /**
     * @param  tileId A TileGridMap tile identifier
     * @return A TileInfo mapped to the supplied TileId
     */
    const Tile& tileInfo(TileId tileId) const;
    /**
     * Attaches a SpriteInstance to the Stage
     *
     * @param  instance The instance to attach from the Stage
     */
    void attachSpriteInstance(SpriteInstancePtr instance);
    /**
     * Detaches the SpriteInstance from the Wrold
     *
     * @param  instance The instance to detach from the Stage
     */
    void detachSpriteInstance(SpriteInstancePtr instance);

    /**
     * Selects ViewModel (SpriteInstance reference) lists from the Stage that
     * intersect a supplied AABB.
     *
     * This method will select instance lists.  Each instance list has its own
     * AABB, and this method will choose one or more instance lists that
     * intersects with the given bounds.  The caller will have to cull
     * individual ViewModel (SpriteInstance) references from the returned lists
     * if it wants a precise result of all instances within the bounds.
     *
     * @param bounds A bounding box that intersects one or more Stage sections.
     *               Each section has its own AABB and instance list.  See the
     *               method definition for details on this process.
     * @param cb     Callback issued for each selected SpriteInstanceList
     */
    void selectInstanceLists(const AABB<Point>& bounds,
                             std::function<void(const SpriteInstanceList&)> cb) const;

private:
    const GameTemplates& _gameTemplates;
    const StaticWorldMap& _staticWorldMap;

    SpriteInstanceList _sprites;
};


} /* namespace overview */ } /* namespace cinek */

#endif

