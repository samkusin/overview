///
/// @file
/// Header documentation for an engine component
///
/// @author     Samir Sinha
/// @date       05/24/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Model_Tile_hpp
#define Overview_Model_Tile_hpp

#include "Engine/Model/ModelTypes.hpp"
#include "Engine/Model/AABB.hpp"

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
    cinek_bitmap bitmap = {
        kCinekBitmapAtlas_Invalid,
        kCinekBitmapIndex_Invalid
    };
    uint32_t flags = 0;         ///< Application defined flags for the tile
    AABB<Point> aabb;           ///< An axis-aligned bounding box for the tile
    glm::ivec2 anchor;          ///< 2D Anchor from tile origin
};

} /* namespace overview */ } /* namespace cinek */

#endif
