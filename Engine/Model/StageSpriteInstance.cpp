/**
 * \file    Model/StageSpriteInstance.cpp
 *
 * Contains a Sprite Model instance's state information.
 *
 * \note    Created by Samir Sinha on 5/27/2014.
 *          Copyright (c) 2014 Cinekine. All rights reserved.
 */

#include "StageSpriteInstance.hpp"
#include "Stage.hpp"

namespace cinekine {
    namespace ovengine {

StageSpriteInstance::StageSpriteInstance(const Sprite& spriteTemplate,
                                         Stage& stage,
                                         SpriteInstanceId instanceId) :
    SpriteInstance(spriteTemplate),
    _stage(stage),
    _thisInstanceId(instanceId),
    _prevInstanceId(0),
    _nextInstanceId(0)
{

}

StageSpriteInstance::~StageSpriteInstance()
{
}

StageSpriteInstance::StageSpriteInstance(StageSpriteInstance&& other) :
    SpriteInstance(std::move(other)),
    _stage(other._stage),
    _pos(other._pos),
    _thisInstanceId(other._thisInstanceId),
    _prevInstanceId(other._prevInstanceId),
    _nextInstanceId(other._nextInstanceId)
{
    other._thisInstanceId = other._prevInstanceId = other._nextInstanceId = 0;
    other._pos = glm::vec3();
}

void StageSpriteInstance::setPosition(const glm::vec3& pos)
{
    uint32_t oldrow = (uint32_t)_pos.x;
    uint32_t oldcol = (uint32_t)_pos.y;
    uint32_t newrow = (uint32_t)pos.x;
    uint32_t newcol = (uint32_t)pos.y;

    if (oldrow != newrow || oldcol != newcol)
    {
        detachFromStage();
        _pos = pos;
        attachToStage();
    }
    else
    {
        _pos = pos;
    }
}

void StageSpriteInstance::detachFromStage()
{
    auto tilemap = _stage.tileGridAtZ((int)_pos.z);
    if (!tilemap)
        return;
    uint32_t oldrow = (uint32_t)_pos.x;
    uint32_t oldcol = (uint32_t)_pos.y;
    //  detach ourselves from our current stage tile
    if (oldrow < tilemap->rowCount() && oldcol < tilemap->columnCount())
    {
        auto& tile = tilemap->at(oldrow, oldcol);

        //  extract ourselves, maintaining a circular list
        auto prevInstance = _stage.spriteInstance(_prevInstanceId);
        if (prevInstance)
            prevInstance->_nextInstanceId = _nextInstanceId;
        auto nextInstance = _stage.spriteInstance(_nextInstanceId);
        if (nextInstance)
            nextInstance->_prevInstanceId = _prevInstanceId;

        //  update our tile's head instance
        if (tile.firstSpriteInstanceId == _thisInstanceId)
        {
            tile.firstSpriteInstanceId = _thisInstanceId == _nextInstanceId ? 0 : _nextInstanceId;
        }

        _prevInstanceId = 0;
        _nextInstanceId = 0;
    }
}

void StageSpriteInstance::attachToStage()
{
    auto tilemap = _stage.tileGridAtZ((int)_pos.z);
    if (!tilemap)
        return;

    uint32_t newrow = (uint32_t)_pos.x;
    uint32_t newcol = (uint32_t)_pos.y;
    //  attach ourselves to our new stage tile
    if (newrow < tilemap->rowCount() && newcol < tilemap->columnCount())
    {
        auto& tile = tilemap->at(newrow, newcol);
        //  insert ourselves to the tail of the list, maintaining a circular list
        if (!tile.firstSpriteInstanceId)
        {
            tile.firstSpriteInstanceId = _thisInstanceId;
            _nextInstanceId = _prevInstanceId = _thisInstanceId;
            return;
        }
        auto headInstance = _stage.spriteInstance(tile.firstSpriteInstanceId);
        if (!headInstance)
        {
            //  error condition, this should never happen...
            tile.firstSpriteInstanceId = 0;
            attachToStage();
            return;
        }
        //  attach ourselves to the tail, and point to the head
        auto lastInstance = _stage.spriteInstance(headInstance->_prevInstanceId);
        lastInstance->_nextInstanceId = _thisInstanceId;
        headInstance->_prevInstanceId = _thisInstanceId;
        _nextInstanceId = headInstance->_thisInstanceId;
        _prevInstanceId = lastInstance->_thisInstanceId;
    }
}

void StageSpriteInstance::reset()
{
    detachFromStage();
    _nextInstanceId = 0;
    _prevInstanceId = 0;
    _thisInstanceId = 0;
}

    } /* rendermodel */
} /* cinekine */