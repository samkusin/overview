//
//  EntityStore.hpp
//  Overview
//
//  Created by Samir Sinha on 5/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Store_hpp
#define Overview_Entity_Store_hpp

#include "EntityTypes.hpp"
#include "EntityDataTable.hpp"

#include <cinek/vector.hpp>
#include <cinek/map.hpp>
#include <cinek/allocator.hpp>

namespace cinek { namespace overview {

// using techniques from:
// http://bitsquid.blogspot.com/2014/08/building-data-oriented-entity-system.html
//
class EntityStore
{
    CK_CLASS_NON_COPYABLE(EntityStore);
    
public:
    EntityStore();
    
    EntityStore(uint32_t numEntities,
        const vector<std::pair<component::Descriptor, uint32_t>>& components,
        const Allocator& allocator=Allocator());
    
    EntityStore(EntityStore&& other);
    EntityStore& operator=(EntityStore&& other);
    
    uint32_t capacity() const { return (uint32_t)_iterations.capacity(); }
    
    Entity create();
    void destroy(Entity eid);
    
    bool valid(Entity eid) const;
    
    //  garbage collects unused components - run once per frame
    void gc();

    template<typename Component> component::Table<Component> table() const;
    
private:
    //  objects indexed by the offset value of the EntityId
    vector<uint16_t> _iterations;
    vector<uint32_t> _freed;
    uint16_t _entityIdIteration;
    unordered_map<ComponentId, component::EntityDataTable> _components;
};

template<typename Component>
component::Table<Component> EntityStore::table() const
{
    auto componentIt = _components.find(Component::kComponentId);
    if (componentIt == _components.end())
        return nullptr;
    const component::EntityDataTable& table = componentIt->second;
    return component::Table<Component>(const_cast<component::EntityDataTable*>(&table));
}

} /* namespace overview */ } /* namespace cinek */

#endif /* defined(Overview_Entity_Database_hpp) */