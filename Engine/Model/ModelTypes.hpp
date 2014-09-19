///
/// @file
/// Common types for all Model objects
///
/// @author     Samir Sinha
/// @date       05/24/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Model_ModelTypes_hpp
#define Overview_Model_ModelTypes_hpp

#include "cinek/types.hpp"
#include "cinek/rendertypes.h"

#include <glm/glm.hpp>

namespace cinekine { namespace ovengine {

    using TileSlot = uint16_t;
    using TileIndex = uint16_t;
    using TileId = uint16_t;
    using SpriteInstanceId = uint32_t;

    using Point = glm::vec3;

    /** Identifies the animation of a particular model. */
    using AnimationStateId = uint16_t;

    /** Define for a null sprite animation template. */
    const AnimationStateId kNullAnimation = (AnimationStateId)0;

    struct TileHandle
    {
        int32_t x;
        int32_t y;
        int32_t layerIndex;     /**< Maps to a TileGridMap layer */
    };

    enum class Quadrant
    {
        kTopLeft,
        kTopRight,
        kBottomRight,
        kBottomLeft,
        kCount
    };

} /* namespace ovengine */ } /* namespace cinekine */

#endif
