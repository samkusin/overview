//
//  TransformEntity.hpp
//  Overview
//
//  Created by Samir Sinha on 7/9/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Transform_Function_hpp
#define Overview_Entity_Transform_Function_hpp

#include "Engine/Entity/EntityTypes.hpp"
#include "Engine/Entity/Comp/Transform.hpp"

#include <ckentity/entitydatatable.hpp>

namespace cinek { namespace overview {

struct UpdateTransformComponent
{
    UpdateTransformComponent() {}
    UpdateTransformComponent(component::Table<TransformComponent> transforms) :
        _transforms(transforms)
    {
    }
    
    ckm::mat4& operator()(Entity entity);
    
    void all();
    
private:
    component::Table<TransformComponent> _transforms;
    Entity _targetEntity;
    ckm::mat4 _srt;
    
    void runTransform(Entity entity, TransformComponent& t);
    bool runTransform(Entity entity, TransformComponent& t, const ckm::mat4& srtParent);
};


////////////////////////////////////////////////////////////////////////////


template<typename Impl>
struct TransformComponentVisitor
{
    TransformComponentVisitor(component::Table<TransformComponent> transforms);
    
protected:
    component::Table<TransformComponent> transforms() { return _transforms; }
    
    bool visitTree(Entity e);
    
private:
    component::Table<TransformComponent> _transforms;
    bool visitTree(Entity entity, TransformComponent& transform);
};

////////////////////////////////////////////////////////////////////////////

template<typename Impl>
TransformComponentVisitor<Impl>::TransformComponentVisitor
(
    component::Table<TransformComponent> transforms
) :
    _transforms(transforms)
{
}

template<typename Impl>
bool TransformComponentVisitor<Impl>::visitTree(Entity entity)
{
    auto* transform = _transforms.dataForEntity(entity);
    if (!transform)
        return false;
        
    visitTree(entity, *transform);
    return true;
}

template<typename Impl>
bool TransformComponentVisitor<Impl>::visitTree
(
    Entity e,
    TransformComponent& t
)
{
    bool res = static_cast<Impl*>(this)->visit(e, t);
    if (!res)
        return false;
    
    //  iterate through children
    Entity child = t.child();
    while (child)
    {
        //  we assume that if there are valid entities set for child and
        //  sibling, that they have transform components.  if this
        //  assumption fails, then there's a problem with how entities
        //  and their parent-child-sibling relationships are cleaned up
        //  upon entity destruction.
        auto childTransform = _transforms.dataForEntity(child);
        if (!visitTree(child, *childTransform))
            return false;
        
        child = _transforms.dataForEntity(child)->sibling();
    }
    return true;
}

} /* namespace overview */ } /* namespace cinek */

#endif
