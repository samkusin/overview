//
//  TransformEntity.cpp
//  Overview
//
//  Created by Samir Sinha on 7/9/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "TransformEntity.hpp"

namespace cinek { namespace overview {

namespace component
{
    Transform* TransformEntity::operator()
    (
        Entity entity,
        Table<Transform> transforms
    )
    {
        _transforms = transforms;
        
        Transform* transform = _transforms.dataForEntity(entity);
        if (transform && transform->dirty())
        {
            const Transform* parentTransform = transform->parent().valid() ?
                _transforms.dataForEntity(transform->parent()) :
                nullptr;
            auto& parentSRT = parentTransform ? parentTransform->worldSRT() : ckm::mat4(1);
            
            runTransform(*transform, parentSRT);
        }
        return transform;
    }
        
    void TransformEntity::all
    (
        Table<Transform> transforms
    )
    {
        _transforms = transforms;
        
        //  only run our recursive transform on root entities (no parents).
        //  for each root entity, descend down the tree and any dirty children
        //  will be transformed.   this prevents children from being transformed
        //  twice, if the child transform is processed before a dirty parent
        
        _transforms.forEach([this](Entity e, Transform& t)
            {
                ckm::mat4 iden(1);
                if (t.parent() == nullptr)
                {
                    runTransform(t, iden);
                }
            });
    }
    
    
    void TransformEntity::runTransform
    (
        Transform& transform,
        const ckm::mat4& parentSRT
    )
    {
        transform.calculateWorldSRTFromParent(parentSRT);
        
        //  iterate through children
        Entity child = transform.child();
        while (child.valid())
        {
            //  we assume that if there are valid entities set for child and
            //  sibling, that they have transform components.  if this
            //  assumption fails, then there's a problem with how entities
            //  and their parent-child-sibling relationships are cleaned up
            //  upon entity destruction.
            auto childTransform = _transforms.dataForEntity(child);
            runTransform(*childTransform, transform.worldSRT());
            child = _transforms.dataForEntity(child)->sibling();
        }
    }
    

    bool TransformVisitor::operator()
    (
        Entity entity,
        Table<Transform> transforms
    )
    {
        _transforms = transforms;
        
        Transform* transform = _transforms.dataForEntity(entity);
        if (!transform)
            return true;
            
        return visitLocal(entity, *transform);
    }
    
    bool TransformVisitor::visitLocal(Entity e, Transform& t)
    {
        if (!visit(e, t))
            return false;
        
        //  iterate through children
        Entity child = t.child();
        while (child.valid())
        {
            //  we assume that if there are valid entities set for child and
            //  sibling, that they have transform components.  if this
            //  assumption fails, then there's a problem with how entities
            //  and their parent-child-sibling relationships are cleaned up
            //  upon entity destruction.
            auto childTransform = _transforms.dataForEntity(child);
            if (!visitLocal(child, *childTransform))
                return false;
            
            child = _transforms.dataForEntity(child)->sibling();
        }
        return true;
    }
    
}

} /* namespace overview */ } /* namespace cinek */
