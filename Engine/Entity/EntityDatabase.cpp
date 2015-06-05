//
//  EntityDatabase.cpp
//  Overview
//
//  Created by Samir Sinha on 5/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "EntityDatabase.hpp"

#include "Engine/Debug.hpp"

namespace cinek { namespace overview {

EntityDatabase::EntityDatabase
(
    uint32_t numEntities,
    const vector<std::pair<component::Descriptor, uint32_t>>& components,
    const Allocator& allocator
) :
    _allocator(allocator),
    _objects(allocator),
    _freeObjectIndices(allocator),
    _entityIdIteration(0),
    _components(allocator)
{
    _objects.reserve(numEntities);
    _freeObjectIndices.reserve(numEntities);
    
    for (auto& component : components)
    {
        component::DataRowset rowset(component.first, component.second, allocator);
        _components.emplace(component.first.id, std::move(rowset));
    }
}
    
EntityObject* EntityDatabase::createEntity()
{
    uint32_t index;
    if (!_freeObjectIndices.empty())
    {
        index = _freeObjectIndices.back();
        _freeObjectIndices.pop_back();
    }
    else
    {
        index = (uint32_t)_objects.size();
        if (index >= _objects.capacity())
        {
            OVENGINE_LOG_WARN("EntityDatabase.createEntity - "
                              "entity count exceeding limit (%u)",
                              _objects.capacity());
            _objects.reserve(_objects.size() + _objects.size()/2);
            _freeObjectIndices.reserve(_objects.capacity());
            if (_objects.capacity() <= _objects.size() ||
                _freeObjectIndices.capacity() <= _objects.size())
            {
                OVENGINE_LOG_ERROR("EntityDatabase.createEntity - "
                                   "Failed to resize entity object container");
                return nullptr;
            }
        }
    }
    
    ++_entityIdIteration;
    if (!_entityIdIteration)
        _entityIdIteration = 1;
    
    EntityId eid = makeEntityId(_entityIdIteration, index);
    if (index == _objects.size())
    {
        _objects.emplace_back(eid, _allocator);
    }
    else
    {
        _objects.emplace(_objects.begin()+index, eid, _allocator);
    }
    
    return &_objects[index];
}

void EntityDatabase::destroyEntity(EntityId eid)
{
    if (!eid)
        return;
    
    auto index = entityIndexFromId(eid);
    
    CK_ASSERT(index < _objects.size());
    
    EntityObject& obj = _objects[index];
    if (obj.id() != eid)
    {
        OVENGINE_LOG_ERROR("EntityDatabase.destroyEntity[%" PRIu64 "] - "
                           "Attemping to destroy an unallocated entity of "
                           "index [%u]",
                           eid, index);
        return;
    }
    
    //  destroy components, then the clear the object
    for (auto& component : obj._components)
    {
        removeComponentRow(component.first, component.second);
    }

    CK_ASSERT(_freeObjectIndices.size() < _freeObjectIndices.capacity());
    
    obj = std::move(EntityObject());
    _freeObjectIndices.push_back(index);
}
    
EntityObject* EntityDatabase::entityObjectFromId(EntityId eid)
{
    return const_cast<EntityObject*>(
        static_cast<const EntityDatabase*>(this)->entityObjectFromId(eid)
    );
}

const EntityObject* EntityDatabase::entityObjectFromId(EntityId eid) const
{
    if (!eid)
        return nullptr;
    
    auto* object = &_objects[entityIndexFromId(eid)];
    if (object->id() != eid)
    {
        OVENGINE_LOG_ERROR("EntityDatabase.entityObjectFromId[%" PRIu64 "] - "
                           "Attempting to retrieve an entity with an invalid ID.",
                           eid);
        return nullptr;
    }
    
    return object;
}
    
void* EntityDatabase::componentFromIndex
(
    ComponentId compId,
    ComponentRowIndex compRowIdx
)
{
    auto componentIt = _components.find(compId);
    if (componentIt == _components.end())
           return nullptr;
    component::DataRowset& rowset = componentIt->second;
    return rowset[compRowIdx];
}

ComponentRowIndex EntityDatabase::addComponentRow(ComponentId compId, EntityId eid)
{
    auto componentIt = _components.find(compId);
    if (componentIt == _components.end())
        return kNullComponentRow;
    
    component::DataRowset& rowset = componentIt->second;
    return rowset.allocate(eid);
}

void EntityDatabase::removeComponentRow
(
    ComponentId compId,
    ComponentRowIndex index
)
{
    auto componentIt = _components.find(compId);
    if (componentIt == _components.end())
        return;
    
    component::DataRowset& rowset = componentIt->second;
    rowset.free(index);
}

void EntityDatabase::removeComponentFromEntityById(EntityId eid, ComponentId id)
{
    EntityObject* obj = entityObjectFromId(eid);
    if (!obj)
        return;
    
    auto index = obj->removeComponent(id);
    removeComponentRow(id, index);;
}

////////////////////////////////////////////////////////////////////////////////
//  built-ins - specialized versions of templated method
//
template<> component::Renderable*
EntityDatabase::addComponentToEntity<component::Renderable>(EntityId eid)
{
    return componentFromEntity<component::Renderable>(eid);
}

template<> component::Transform*
EntityDatabase::addComponentToEntity<component::Transform>(EntityId eid)
{
    return componentFromEntity<component::Transform>(eid);
}

template<> component::Renderable*
EntityDatabase::componentFromEntity<component::Renderable>(EntityId eid)
{
    return const_cast<component::Renderable*>(
        static_cast<EntityDatabase*>(this)->componentFromEntity<component::Renderable>(eid)
    );
}

template<> const component::Renderable*
EntityDatabase::componentFromEntity<component::Renderable>(EntityId eid) const
{
    const EntityObject* obj = entityObjectFromId(eid);
    if (!obj)
        return nullptr;
    return &obj->renderable();
}

template<> component::Transform*
EntityDatabase::componentFromEntity<component::Transform>(EntityId eid)
{
    return const_cast<component::Transform*>(
        static_cast<EntityDatabase*>(this)->componentFromEntity<component::Transform>(eid)
    );
}

template<> const component::Transform*
EntityDatabase::componentFromEntity<component::Transform>(EntityId eid) const
{
    const EntityObject* obj = entityObjectFromId(eid);
    if (!obj)
        return nullptr;
    return &obj->transform();
}


} /* namespace overview */ } /* namespace cinek */