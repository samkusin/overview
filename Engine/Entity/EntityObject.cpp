//
//  EntityObject.cpp
//  Overview
//
//  Created by Samir Sinha on 5/20/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "EntityObject.hpp"

namespace cinek { namespace overview {

EntityObject::EntityObject(EntityId eid, const Allocator& allocator) :
    _eid(eid),
    _components(allocator)
{
}

EntityObject::EntityObject(EntityObject&& other) :
    _eid(other._eid),
    _transform(other._transform),
    _renderable(other._renderable),
    _components(std::move(other._components))
{
    other._eid = kNullEntity;
}

EntityObject& EntityObject::operator=(EntityObject&& other)
{
    _eid = other._eid;
    other._eid = kNullEntity;
    _transform = other._transform;
    _renderable = other._renderable;
    _components = std::move(other._components);
    
    return *this;
}

ComponentRowIndex EntityObject::componentIndexFromId(ComponentId id) const
{
    //  binary search for the index
    //  TODO: perhaps for small lists, find will be better than lower_bound -
    //  let's come back to this lower_bound vs find problem if it becomes an
    //  issue
    
    auto it = std::lower_bound(_components.begin(), _components.end(), id,
        [](const decltype(_components)::value_type& entry, uint32_t id) -> bool {
            return entry.first < id;
        });
    if (it == _components.end() || it->first != id)
        return kNullComponentRow;
    
    return it->second;
}

void EntityObject::addComponent(ComponentId id, ComponentRowIndex index)
{
    auto it = std::lower_bound(_components.begin(), _components.end(), id,
        [](const decltype(_components)::value_type& entry, uint32_t id) -> bool {
            return entry.first < id;
        });
    if (it == _components.end() || it->first != id)
    {
        _components.insert(it, std::make_pair(id, index));
    }
}

ComponentRowIndex EntityObject::removeComponent(ComponentId id)
{
    auto it = std::lower_bound(_components.begin(), _components.end(), id,
        [](const decltype(_components)::value_type& entry, uint32_t id) -> bool {
            return entry.first < id;
        });
    if (it == _components.end() || it->first != id)
        return kNullComponentRow;
    ComponentRowIndex index = it->second;
    _components.erase(it);
    return index;
}

} /* namespace overview */ } /* namespace cinek */

