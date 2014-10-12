///
/// @file
/// Common types for all Builder objects
///
/// @author     Samir Sinha
/// @date       07/16/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Builder_Types_Hpp
#define Overview_Builder_Types_Hpp

#include "Engine/Model/StageTypes.hpp"
#include "Engine/Model/AABB.hpp"
#include "Engine/Model/Tile.hpp"

namespace cinekine { namespace ovengine {

/// @defgroup Builder The Map Grid Builder
/// The Builder is a toolbox used by map generators to procedurally create
/// tile grids onto Stages.
///

enum BuilderPaintStyle
{
    kBuilderPaintStyle_Tiled,
    kBuilderPaintStyle_Stretch
};

typedef uint16_t BlockCollectionId;

} /* namespace overview */ } /* namespace cinekine */

#endif