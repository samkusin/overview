//
//  EntityGroupMap.cpp
//  Overview
//
//  Created by Samir Sinha on 7/25/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "EntityGroupMap.hpp"
#include "EntityDataTable.hpp"

#include "Engine/Debug.hpp"

namespace cinek { namespace overview {


EntityGroupMap::EntityGroupMap
(
    const MakeDescriptor& makeDesc,
    const Allocator& allocator
) :
    _rowset(makeDesc.cnt, makeDesc.roleLimits, allocator),
    _entityToGroupIndex(allocator),
    _id(makeDesc.id)
{
}

auto EntityGroupMap::allocateIndexForEntity(Entity entity) -> index_type
{
    CK_ASSERT_RETURN_VALUE(entity.valid(), npos);

    auto idx = _rowset.allocate(entity);
    CK_ASSERT_RETURN_VALUE(idx != npos, npos);
    if (idx == npos)
        return npos;
    
    auto res = _entityToGroupIndex.emplace(entity, idx);
    if (res.first == _entityToGroupIndex.end())
        return npos;
    CK_ASSERT(res.second);  // two rows belonging to same entity?!

    return idx;
}
    

void EntityGroupMap::removeDataFromEntity(Entity entity)
{
    if (!entity.valid())
        return;
    
    auto it = _entityToGroupIndex.find(entity);
    if (it == _entityToGroupIndex.end())
        return;
 
    uint32_t idx = it->second;
    _entityToGroupIndex.erase(it);
    _rowset.free(idx);
}
    
auto EntityGroupMap::rowIndexFromEntity(Entity entity) const -> index_type
{
    if (!entity.valid())
        return npos;
    
    auto it = _entityToGroupIndex.find(entity);
    if (it == _entityToGroupIndex.end())
        return npos;

    uint32_t idx = it->second;
    CK_ASSERT(idx < _rowset.size());
    return idx;
}

namespace component
{

//template<> class Table<EntityGroup, EntityGroupMap>;

//  specialization of constructor - no need to verify ComponentId of dataTable
//  matches the Component (which is the EntityGroup object - a dynamic type)
template<>
Table<EntityGroup, EntityGroupMap>::Table(EntityGroupMap* dataTable) :
    _table(dataTable)
{

}


}

} /* namespace overview */ } /* namespace cinek */