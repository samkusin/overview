//
//  EntityDatabase.cpp
//  Overview
//
//  Created by Samir Sinha on 5/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "EntityStore.hpp"

#include "Engine/Debug.hpp"

namespace cinek { namespace overview {

EntityStore::EntityStore() :
    _entityIdIteration(0),
    _entityCount(0)
{
}

EntityStore::EntityStore
(
    uint32_t numEntities,
    const vector<component::MakeDescriptor>& components,
    const vector<EntityGroupMap::MakeDescriptor>& entityGroups,
    const Allocator& allocator
) :
    _iterations(allocator),
    _freed(allocator),
    _entityIdIteration(0),
    _entityCount(0),
    _components(allocator)
{
    _iterations.reserve(numEntities);
    _freed.reserve(numEntities);
    for (auto& component : components)
    {
        component::EntityDataTable rowset(component, allocator);
        _components.emplace(component.desc.id, std::move(rowset));
    }
    for (auto& group : entityGroups)
    {
        EntityGroupMap rowset(group, allocator);
        _entityGroups.emplace(group.id, std::move(rowset));
    }
}


EntityStore::EntityStore(EntityStore&& other) :
    _iterations(std::move(other._iterations)),
    _freed(std::move(other._freed)),
    _entityIdIteration(other._entityIdIteration),
    _components(std::move(other._components))
{
    other._entityIdIteration = 0;
}

EntityStore& EntityStore::operator=(EntityStore&& other)
{
    _iterations = std::move(other._iterations);
    _freed = std::move(other._freed);
    _entityIdIteration = other._entityIdIteration;
    _entityCount = other._entityCount;
    _components = std::move(other._components);
    
    other._entityIdIteration = 0;
    other._entityCount = 0;
    
    return *this;
}
    
Entity EntityStore::create(Entity::context_type context)
{
    Entity::index_type index;
    
    if (!_freed.empty())
    {
        index = _freed.back();
        _freed.pop_back();
    }
    else
    {
        index = (Entity::index_type)_iterations.size();
        _iterations.push_back(1);
    }
    
    Entity eid;
    eid.makeEntityId(_iterations[index], context, index);
    ++_entityCount;
    return eid;
}

void EntityStore::destroy(Entity eid)
{
    if (eid==0)
        return;
    
    auto index = eid.index();
    ++_iterations[index];
    if (!_iterations[index])
        _iterations[index] = 1;

    --_entityCount;

    _freed.push_back(index);
}

bool EntityStore::valid(Entity eid) const
{
    return (_iterations[eid.index()] == eid.iteration());
}

void EntityStore::gc()
{
    //TODO
}

component::EntityGroupTable EntityStore::entityGroupTable(EntityGroupMapId id) const
{
    auto it = _entityGroups.find(id);
    if (it == _entityGroups.end())
        return component::EntityGroupTable();
    
    const EntityGroupMap& entityGroupMap = it->second;
    return component::EntityGroupTable(const_cast<EntityGroupMap*>(&entityGroupMap));
}

void EntityStore::diagnostics(EntityDiagnostics& diagnostics)
{
    diagnostics.components.reserve(_components.size());
    diagnostics.components.clear();
    
    for (auto& component : _components)
    {
        diagnostics.components.emplace_back();
        
        auto& diag = diagnostics.components.back();
        diag.name = component.second.name();
        diag.allocated = component.second.usedCount();
        diag.limit = component.second.rowset().capacity();
    }
    
    diagnostics.groups.reserve(_entityGroups.size());
    diagnostics.groups.clear();
    
    for (auto& group : _entityGroups)
    {
        diagnostics.groups.emplace_back();
    
        auto& diag = diagnostics.groups.back();
        diag.id = group.first;
        diag.allocated = group.second.usedCount();
        diag.limit = group.second.rowset().capacity();
    }
    
    diagnostics.groups.reserve(_entityGroups.size());
    
    diagnostics.entityCount = _entityCount;
    diagnostics.entityLimit = (uint32_t)_iterations.capacity();
}

} /* namespace overview */ } /* namespace cinek */