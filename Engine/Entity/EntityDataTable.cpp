//
//  EntityDataTable.cpp
//  Overview
//
//  Created by Samir Sinha on 6/19/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "EntityDataTable.hpp"

namespace cinek { namespace overview {

namespace component
{

EntityDataTable::EntityDataTable
(
    const Descriptor& desc,
    uint32_t rowCount,
    const Allocator& allocator
) :
    _descriptor(desc),
    _entityToRow(allocator),
    _rowset(desc, rowCount, allocator)
{
}

auto EntityDataTable::allocateIndexForEntity(Entity eid) -> index_type
{
    auto indexIt = _entityToRow.find(eid);
    if (indexIt == _entityToRow.end())
    {
        auto idx = _rowset.allocate(eid);
        _descriptor.initCb(_rowset.at(idx));
        indexIt = _entityToRow.insert(std::make_pair(eid, idx)).first;
    }
    return indexIt->second;
}

void EntityDataTable::removeDataFromEntity
(
    Entity eid
)
{
    auto indexIt = _entityToRow.find(eid);
    if (indexIt == _entityToRow.end())
        return;
    
    auto index = indexIt->second;
    _descriptor.termCb(_rowset.at(index));
    _rowset.free(index);
}

}

} /* namespace overview */ } /* namespace cinek */
