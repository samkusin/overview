///
/// @file Tile/Tile.hpp
/// Header documentation for an engine component
///
/// @author     Samir Sinha
/// @date       05/24/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Model_Tile_hpp
#define Overview_Model_Tile_hpp

#include "Engine/ModelTypes.hpp"
#include "Engine/AABB.hpp"
#include "CKGfx/GfxTypes.hpp"

namespace cinek { namespace overview {

/// @defgroup TileModel The Tile Model
/// The Tile Model encompasses all objects related to housekeeping and rendering
/// of 2D Tiles.
///

/// @struct Tile
/// @ingroup TileModel
/// @brief  The visual and physical representation of a map tile
///
struct Tile
{
    /// Points to the bitmap representing the tile
    gfx::BitmapHandle bitmap;
    uint32_t categories = 0;    ///< Application defined flags for the tile
    AABB<Point> aabb;           ///< An axis-aligned bounding box for the tile
    glm::ivec2 anchor;          ///< 2D Anchor from tile origin

    CollisionInfo collision;    ///< Information used by the Collision system
};

} /* namespace overview */ } /* namespace cinek */

#endif
