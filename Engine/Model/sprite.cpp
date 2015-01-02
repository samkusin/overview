/**
 * \file    Model/Sprite.cpp
 *
 * Model data shared across
 *
 * \note    Created by Samir Sinha on 5/18/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "Sprite.hpp"

#include <algorithm>
#include <cstring>

namespace cinek {
    namespace overview {

Sprite::Sprite() :
    _bitmapClass(kCinekBitmapAtlas_Invalid),
    _statePool(1, _allocator),
    _states(_allocator)
{
}

Sprite::Sprite(const std::string& name,
               cinek_bitmap_atlas bitmapClass,
               const glm::ivec2& anchor,
               const AABB<Point>& aabb,
               uint16_t numStates,
               const Allocator& allocator) :
    _allocator( allocator ),
    _name(name),
    _bitmapClass( bitmapClass ),
    _anchor( anchor ),
    _aabb(aabb),
    _statePool( numStates, allocator ),
    _states(_allocator)
{
    _states.reserve(numStates);
}

Sprite::~Sprite()
{
    for( auto& state : _states )
    {
        _allocator.free(state->_frames);
    }
}

Sprite::Sprite(Sprite&& other) :
    _allocator(std::move(other._allocator)),
    _name(std::move(other._name)),
    _bitmapClass(other._bitmapClass),
    _anchor(std::move(other._anchor)),
    _aabb(std::move(other._aabb)),
    _statePool(std::move(other._statePool)),
    _states(std::move(other._states))
{
    other._bitmapClass = kCinekBitmapAtlas_Invalid;
}

Sprite& Sprite::operator=(Sprite&& other)
{
    _allocator = std::move(other._allocator);
    _name = std::move(other._name);
    _bitmapClass = other._bitmapClass;
    _anchor = std::move(other._anchor);
    _aabb = std::move(other._aabb);
    _statePool = std::move(other._statePool);
    _states = std::move(other._states);

    other._bitmapClass = kCinekBitmapAtlas_Invalid;
    return *this;
}

struct SpriteState_Comparator
{
    bool operator()(const SpriteAnimation* state, const AnimationStateId& id) const
    {
        return state->getId() < id;
    }
};

SpriteAnimation* Sprite::createAnimation(
                            AnimationStateId animId,
                            uint16_t frameCount,
                            uint32_t duration
                        )
{
    //  maintain a sorted state vector.
    auto stateIt = std::lower_bound(_states.begin(), _states.end(), animId, SpriteState_Comparator());
    SpriteAnimation* state = nullptr;
    if (stateIt == _states.end() || (*stateIt)->getId() != animId)
    {
        cinek_bitmap_index* frames = _allocator.allocItems<cinek_bitmap_index>(frameCount);
        state = _statePool.allocateAndConstruct(animId, frameCount, frames, duration);
        _states.emplace(stateIt, state);
    }
    else
    {
        state = (*stateIt);
        _allocator.free(state->_frames);
        state->~SpriteAnimation();
        cinek_bitmap_index* frames = _allocator.allocItems<cinek_bitmap_index>(frameCount);
        ::new(state) SpriteAnimation(animId, frameCount, frames, duration);
    }

    return state;
}

SpriteAnimation* Sprite::getAnimation(AnimationStateId animId) const
{
    auto stateIt = std::lower_bound(_states.begin(), _states.end(), animId, SpriteState_Comparator());
    if (stateIt != _states.end() && (*stateIt)->getId() == animId)
    {
        return *stateIt;
    }
    return nullptr;
}

    }   // namespace overview
}   // namespace cinek
