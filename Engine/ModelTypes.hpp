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

#include <cinek/types.hpp>
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

    /**
     *  A generic model identifier used by clients.  Its defined
     *  for applications to pass model identifiers across systems.
     *  It is up to the application to define how this identifier
     *  gets mapped to tiles, sprites, or whatever model types the
     *  title supports.
     */
    using CommonModelId = uint32_t;

    struct CollisionInfo
    {
        enum class Shape
        {
            kNone,
            kPlane,
            kBox
        };
        Shape shape = Shape::kNone;
        uint16_t access = 0;
        uint16_t impedence = 0;
    };

} /* namespace overview */ } /* namespace cinek */

#endif
