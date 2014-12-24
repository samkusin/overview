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

namespace cinek { namespace overview {

    using TileSlot = uint16_t;
    using TileIndex = uint16_t;
    using TileId = uint16_t;

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

    enum class CollisionType
    {
        kNone,                  /** Ignored by any collision engine */
        kStatic,                /** Collisions with any non-static object */
        kDynamic                /** Collisions with any object, static, etc. */
    };

} /* namespace overview */ } /* namespace cinek */

#endif
