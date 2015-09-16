//
//  TransformEntity.cpp
//  Overview
//
//  Created by Samir Sinha on 7/9/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "TransformEntity.hpp"
#include "Engine/Debug.hpp"

namespace cinek { namespace overview {

ckm::mat4& UpdateTransformComponent::operator()(Entity entity)
{
    //  navigate up to root parent entity so that we can calculate the
    //  whole transform from root to this entity.
    
    auto xform = _transforms.dataForEntity(entity);
    if (!xform)
    {
        _srt = ckm::mat4(1);
    }
    else
    {
        runTransform(entity, *xform);
    }
    
    return _srt;
}

void UpdateTransformComponent::all()
{
    _transforms.forEach(
        [this](Entity e, TransformComponent& t) {
            if (t.isDirty())
            {
                runTransform(e, t);
            }
        });
}

void UpdateTransformComponent::runTransform
(
    Entity entity,
    TransformComponent& t
)
{
    Entity rootEntity = entity;
    auto rootTransform = &t;
    auto firstDirtyEntity = entity;
    auto firstDirtyTransform = &t;
    ckm::mat4 parentSRT(1);
    
    while (rootTransform && rootTransform->parent())
    {
        bool firstDirtyChange = false;
        if (rootTransform->isDirty())
        {
            firstDirtyEntity = rootEntity;
            firstDirtyTransform = rootTransform;
            firstDirtyChange = true;
        }
        rootEntity = rootTransform->parent();
        rootTransform = _transforms.dataForEntity(rootEntity);
        if (firstDirtyChange)
        {
            if (rootTransform)
                rootTransform->calcMatrix(parentSRT);
            else
                parentSRT = ckm::mat4(1);
        }
    }
    
    //  error condition (unlikely, but handle)
    if (!rootTransform)
    {
        firstDirtyEntity = entity;
        firstDirtyTransform = &t;
        OVENGINE_LOG_ERROR("TransformCreateMatrix() - entity transform tree "
                           "broken for entity %" PRIu64, rootEntity);
    }
    
    _targetEntity = entity;
    
    //  get parent SRT if firstDirtyEntity has one.
    
    runTransform(firstDirtyEntity, *firstDirtyTransform, parentSRT);
}

bool UpdateTransformComponent::runTransform
(
    Entity e,
    TransformComponent& transform,
    const ckm::mat4& srtParent)
{
    ckm::mat4 srt;
    
    if (transform.isDirty())
    {
        _srt = transform.calcLocalMatrix(srt) * srtParent;
        transform.setOrient(ckm::quatFromMtx4x4(_srt));
        transform.setPosition(ckm::vec3(_srt[3]));
        transform.clearDirty();
    }
    else
    {
        _srt = transform.calcMatrix(_srt);
    }

    if (e == _targetEntity)
        return false;
    
    srt = _srt;
    
    //  iterate through children
    Entity child = transform.child();
    while (child)
    {
        //  we assume that if there are valid entities set for child and
        //  sibling, that they have transform components.  if this
        //  assumption fails, then there's a problem with how entities
        //  and their parent-child-sibling relationships are cleaned up
        //  upon entity destruction.
        auto childTransform = _transforms.dataForEntity(child);
        
        if (!runTransform(child, *childTransform, srt))
            return false;
        
        child = _transforms.dataForEntity(child)->sibling();
    }
    
    return true;
}

} /* namespace overview */ } /* namespace cinek */
