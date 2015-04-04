/**
 * \file    Sprite/Sprite.cpp
 *
 * Contains a Sprite Model instance's state information.
 *
 * \note    Created by Samir Sinha on 5/18/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "Engine/Sprite/SpriteInstance.hpp"
#include "Engine/Sprite/Sprite.hpp"
#include "Engine/Sprite/SpriteAnimation.hpp"

namespace cinek {
    namespace overview {

const Sprite SpriteInstance::kNullSprite;

SpriteInstance::SpriteInstance() :
    __prevListNode(nullptr),
    __nextListNode(nullptr),
    _template(kNullSprite),
    _startTime(0),
    _stateId(kNullAnimation),
    _animation(nullptr)
{
}


SpriteInstance::SpriteInstance(const Sprite& spriteTemplate) :
    __prevListNode(nullptr),
    __nextListNode(nullptr),
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
    __prevListNode(other.__prevListNode),
    __nextListNode(other.__nextListNode),
    _template(other._template),
    _startTime(other._startTime),
    _stateId(other._stateId),
    _animation(other._animation)
{
    other.__prevListNode = nullptr;
    other.__nextListNode = nullptr;
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


uint16_t SpriteInstance::bitmapFrameCount() const
{
    return _animation->getFrameCount();
}

cinek_bitmap SpriteInstance::bitmapFromTime(uint32_t currentTime) const
{
    return { _template.getBitmapClass(), _animation->getFrameByTime(currentTime - _startTime) };
}

cinek_bitmap SpriteInstance::bitmapFrame(uint16_t index) const
{
    return { _template.getBitmapClass(), _animation->getFrame(index) };
}

const AABB<Point>& SpriteInstance::aabb() const
{
    return _template.aabb();
}

const glm::ivec2& SpriteInstance::anchor() const
{
    return _template.anchor();
}


    } /* rendermodel */
} /* cinekine */