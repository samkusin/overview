/**
 * @file    rendermodel/spriteanimation.cpp
 *
 * Sprite animation interface
 *
 * @note    Created by Samir Sinha on 12/30/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "SpriteAnimation.hpp"

namespace cinek {
    namespace overview {

    SpriteAnimation::SpriteAnimation(AnimationStateId id, uint16_t frameCount,
                                     cinek_bitmap_index* frames,
                                     uint32_t duration) :
        _id(id),
        _duration(duration),
        _frameCount(frameCount),
        _frames(frames)
    {
    }

    cinek_bitmap_index SpriteAnimation::getFrameByTime(uint32_t timeValue) const
    {
        float scalar = (float)timeValue/_duration;
        return getFrame((uint16_t)(getFrameCount() * scalar));
    }

    }   // namespace overview
}   //  namespace cinek