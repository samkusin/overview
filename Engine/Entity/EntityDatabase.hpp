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
#include "EntityDataTable.hpp"

#include <cinek/map.hpp>
#include <cinek/allocator.hpp>

namespace cinek { namespace overview {

// using techniques from:
// http://bitsquid.blogspot.com/2014/08/building-data-oriented-entity-system.html
//
class EntityDatabase
{
    CK_CLASS_NON_COPYABLE(EntityDatabase);
    
public:        
    EntityDatabase(uint32_t numEntities,
        const vector<std::pair<component::Descriptor, uint32_t>>& components,
        const Allocator& allocator=Allocator());
    
    Entity create();
    void destroy(Entity eid);

    template<typename Component> component::Table<Component> table() const;
    
private:
    //  objects indexed by the offset value of the EntityId
    vector<uint16_t> _iterations;
    vector<uint32_t> _freed;
    uint16_t _entityIdIteration;
    unordered_map<ComponentId, component::EntityDataTable> _components;
};

template<typename Component>
component::Table<Component> EntityDatabase::table() const
{
    auto componentIt = _components.find(Component::kComponentId);
    if (componentIt == _components.end())
        return nullptr;
    const component::EntityDataTable& table = componentIt->second;
    return component::Table<Component>(const_cast<component::EntityDataTable*>(&table));
}

} /* namespace overview */ } /* namespace cinek */

#endif /* defined(Overview_Entity_Database_hpp) */
