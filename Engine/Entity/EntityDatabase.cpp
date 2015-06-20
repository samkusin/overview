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
    
Entity EntityDatabase::create()
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

void EntityDatabase::destroy(Entity eid)
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