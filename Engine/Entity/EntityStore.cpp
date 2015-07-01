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
    _entityIdIteration(0)
{
}

EntityStore::EntityStore
(
    uint32_t numEntities,
    const vector<std::pair<component::Descriptor, uint32_t>>& components,
    const Allocator& allocator
) :
    _iterations(allocator),
    _freed(allocator),
    _entityIdIteration(0),
    _components(allocator)
{
    _iterations.reserve(numEntities);
    _freed.reserve(numEntities);
    for (auto& component : components)
    {
        component::EntityDataTable rowset(component.first, component.second, allocator);
        _components.emplace(component.first.id, std::move(rowset));
    }
}


EntityStore::EntityStore(EntityStore&& other) :
    _iterations(std::move(other._iterations)),
    _freed(std::move(other._freed)),
    _entityIdIteration(other._entityIdIteration),
    _components(std::move(other._entityIdIteration))
{
    other._entityIdIteration = 0;
}

EntityStore& EntityStore::operator=(EntityStore&& other)
{
    _iterations = std::move(other._iterations);
    _freed = std::move(other._freed);
    _entityIdIteration = other._entityIdIteration;
    _components = std::move(other._components);
    
    other._entityIdIteration = 0;
    
    return *this;
}
    
Entity EntityStore::create()
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
    eid.makeEntityId(_iterations[index], index);
    return eid;
}

void EntityStore::destroy(Entity eid)
{
    if (!eid)
        return;
    
    auto index = eid.index();
    ++_iterations[index];
    if (!_iterations[index])
        _iterations[index] = 1;

    _freed.push_back(index);
}


} /* namespace overview */ } /* namespace cinek */