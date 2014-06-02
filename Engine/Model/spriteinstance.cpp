/**
 * \file    rendermodel/sprite.cpp
 *
 * Contains a Sprite Model instance's state information.
 *
 * \note    Created by Samir Sinha on 5/18/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "SpriteInstance.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"

namespace cinekine {
    namespace ovengine {

SpriteInstance::SpriteInstance(const Sprite& spriteTemplate) :
    _template(spriteTemplate),
    _startTime(0),
    _stateId(kNullAnimation),
    _animation(nullptr)
{

}

SpriteInstance::~SpriteInstance()
{
}

SpriteInstance::SpriteInstance(SpriteInstance&& other) :
    _template(other._template),
    _startTime(other._startTime),
    _stateId(other._stateId),
    _animation(other._animation)
{
    other._startTime = 0;
    other._stateId = kNullAnimation;
    other._animation = nullptr;
}

void SpriteInstance::setState(AnimationStateId stateId, uint32_t startTime)
{
    _stateId = stateId;
    _startTime = startTime;

    _animation = _template.getAnimation(_stateId);
}

cinek_bitmap_atlas SpriteInstance::bitmapAtlas() const
{
    return _template.getBitmapClass();
}

uint16_t SpriteInstance::bitmapFrameCount() const
{
    return _animation->getFrameCount();
}

cinek_bitmap_index SpriteInstance::bitmapFromTime(uint32_t currentTime) const
{
    return _animation->getFrameByTime(currentTime - _startTime);
}

cinek_bitmap_index SpriteInstance::bitmapFrame(uint16_t index) const
{
    return _animation->getFrame(index);
}

    } /* rendermodel */
} /* cinekine */