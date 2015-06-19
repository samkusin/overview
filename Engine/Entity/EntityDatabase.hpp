//
//  EntityDatabase.hpp
//  Overview
//
//  Created by Samir Sinha on 5/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Database_hpp
#define Overview_Entity_Database_hpp

#include "EntityTypes.hpp"
#include "EntityObject.hpp"
#include "Comp/DataRowset.hpp"

#include <cinek/map.hpp>
#include <cinek/allocator.hpp>

namespace cinek { namespace overview {

class EntityDatabase
{
    CK_CLASS_NON_COPYABLE(EntityDatabase);
    
public:        
    EntityDatabase(uint32_t numEntities,
        uint16_t id,
        const vector<std::pair<component::Descriptor, uint32_t>>& components,
        const Allocator& allocator=Allocator());
    
    EntityObject* createEntity();
    void destroyEntity(EntityId eid);
    
    EntityObject* entityObjectFromId(EntityId eid);
    const EntityObject* entityObjectFromId(EntityId eid) const;

    template<typename Component> Component* addComponentToEntity(EntityId eid);
    template<typename Component> const Component* componentFromEntity(EntityId eid) const;

    template<typename Component> Component* componentFromEntity(EntityId eid);
    template<typename Component> Component* component(ComponentRowIndex index);
    template<typename Component> const Component* component(ComponentRowIndex index) const;
    void removeComponentFromEntityById(EntityId eid, ComponentId id);
    
    //  iterate through all objects in the DB with the specified component
    //  Function signature should be:
    //      void fn(EntityId eid, Component& component);
    template<typename Fn, typename Component> Fn forEach(Fn fn);
    //  iterate through all objects in the DB - useful for transform and
    //  other such components bound to the EntityObject
    //  Function signature should be:
    //      void fn(EntityObject& object);
    template<typename Fn> Fn forEachObject(Fn fn);
    
private:
    friend class EntityObject;

    void* componentFromIndex(ComponentId compId, ComponentRowIndex index);
    const void* componentFromIndex(ComponentId compId, ComponentRowIndex index) const;
    ComponentRowIndex addComponentRow(ComponentId compId, EntityId eid);
    void removeComponentRow(ComponentId compId, ComponentRowIndex index);
    
    Allocator _allocator;
    
    //  objects indexed by the offset value of the EntityId
    vector<EntityObject> _objects;
    vector<uint32_t> _freeObjectIndices;
    uint16_t _dbId;
    uint16_t _entityIdIteration;
    unordered_map<ComponentId, component::DataRowset> _components;
};

template<typename Component>
Component* EntityDatabase::addComponentToEntity(EntityId eid)
{
    EntityObject* obj = entityObjectFromId(eid);
    if (!obj)
        return nullptr;
    auto index = obj->componentIndexFromId(Component::kComponentId);
    if (index == kNullComponentRow)
    {
        index = addComponentRow(Component::kComponentId, obj->id());
        if (index == kNullComponentRow)
            return nullptr;
        
        obj->addComponent(Component::kComponentId, index);
    }
    return reinterpret_cast<Component* >(componentFromIndex(Component::kComponentId, index));
}

template<typename Component>
Component* EntityDatabase::componentFromEntity(EntityId eid)
{
    return const_cast<Component*>(static_cast<const EntityDatabase*>(this)->componentFromEntity<Component>(eid));
}

template<typename Component>
const Component* EntityDatabase::componentFromEntity(EntityId eid) const
{
    const EntityObject* obj = entityObjectFromId(eid);
    if (!obj)
        return nullptr;
    
    auto index = obj->componentIndexFromId(Component::kComponentId);
    if (index == kNullComponentRow)
        return nullptr;
    
    return component<Component>(index);
}

template<typename Component>
Component* EntityDatabase::component(ComponentRowIndex index)
{
    return const_cast<Component*>(static_cast<EntityDatabase*>(this)->component<Component>(index));
}

template<typename Component>
const Component* EntityDatabase::component(ComponentRowIndex index) const
{
    return reinterpret_cast<const Component*>(componentFromIndex(Component::kComponentId, index));
}

template<typename Fn, typename Component>
Fn EntityDatabase::forEach(Fn fn)
{
    //  partial template specialization might be ideal if it was supported
    //  we'll count on specialized forEach methods for EntityObjects instead.
    static_assert(Component::kComponentId != component::kTransform
                  && Component::kComponentId != component::kRenderable,
                  "Not supported for Transform or Renderable components - Use specialized forEachObject");

    auto it = _components.find(Component::kComponentId);
    if (it != _components.end())
    {
        auto& dataset = it->second;
        auto idx = dataset.firstIndex();
        while (idx != component::DataRowset::npos)
        {
            fn(dataset.entityAt(idx), *dataset.template at<Component>(idx));
            idx = dataset.nextIndex(idx);
        }
    }
    return fn;
}

template<typename Fn>
Fn EntityDatabase::forEachObject(Fn fn)
{
    for (auto& object : _objects)
    {
        if (object.id() != kNullEntity)
        {
            fn(object);
        }
    }
    return fn;
}

} /* namespace overview */ } /* namespace cinek */

#endif /* defined(Overview_Entity_Database_hpp) */
