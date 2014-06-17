//
//  ModelTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 5/24/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Model_ModelTypes_hpp
#define Overview_Model_ModelTypes_hpp

#include "cinek/types.hpp"
#include "cinek/rendertypes.h"

#include <glm/glm.hpp>

namespace cinekine { namespace ovengine {

    typedef uint16_t TileId;
    typedef uint32_t SpriteInstanceId;

    struct AABB
    {
        glm::vec3 min;
        glm::vec3 max;
    };

    /** Identifies the animation of a particular model. */
    typedef uint16_t AnimationStateId;

    /** Define for a null sprite template. */
    const AnimationStateId kNullAnimation = (AnimationStateId)0;

    /** Keyframe information for an animation. */
    struct ModelKeyframe
    {
        uint32_t frame; /**< Frame index. */
        uint32_t t;   /**< Time index at this frame. */
    };

} /* namespace ovengine */ } /* namespace cinekine */

#endif
