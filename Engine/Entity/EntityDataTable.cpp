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
    const MakeDescriptor& desc,
    const Allocator& allocator
) :
    _descriptor(desc.desc),
    _context(desc.context),
    _entityToRow(allocator),
    _rowset(desc.desc, desc.cnt, allocator)
{
}

auto EntityDataTable::allocateIndexForEntity(Entity eid) -> index_type
{
    auto indexIt = _entityToRow.find(eid);
    if (indexIt == _entityToRow.end())
    {
        auto idx = _rowset.allocate(eid);
        _descriptor.initCb(eid, _rowset.at(idx), _context);
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
    _descriptor.termCb(eid, _rowset.at(index), _context);
    _entityToRow.erase(indexIt);
    _rowset.free(index);
}

}

} /* namespace overview */ } /* namespace cinek */
