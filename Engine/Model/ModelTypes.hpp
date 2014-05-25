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

namespace cinekine { namespace ovengine {

    typedef uint16_t TileId;

    struct Tile
    {
        TileId layer[2];
    };

    struct TileInfo
    {
        cinek_bitmap bitmap;            /**< The tile's bitmap */
        uint8_t categoryIndex;
        uint8_t classIndex;
        uint16_t flags;
    };

    /** Identifies the animation of a particular model. */
    typedef uint16_t AnimationStateId;

    /** Identifies a sprite by key ID. */
    typedef uint32_t SpriteId;

    /** Define for a null sprite template. */
    const SpriteId kNullSprite  = (SpriteId)0;

    /** Define for a null sprite template. */
    const AnimationStateId kNullAnimation = (AnimationStateId)0;

    /** Keyframe information for an animation. */
    struct ModelKeyframe
    {
        uint16_t frame; /**< Frame index. */
        uint32_t t;   /**< Time index at this frame. */
    };

} /* namespace ovengine */ } /* namespace cinekine */

#endif
