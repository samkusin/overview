//
//  EntityGroupMap.hpp
//  Overview
//
//  Created by Samir Sinha on 7/25/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_GroupMap_hpp
#define Overview_Entity_GroupMap_hpp

#include "EntityGroup.hpp"
#include "Engine/VectorDataRowset.hpp"

#include <cinek/map.hpp>

namespace cinek { namespace overview {

//  The EntityGroupMap is a container for EntityGroup objects defined by a set
//  of Roles.
//
//  This object preallocates all memory required for the entirety of the Map,
//  which means all EntityGroup objects as well.
//
class EntityGroupMap
{
public:
    using container_type = VectorDataRowset<EntityGroup>;
    using index_type = typename container_type::index_type;
    static constexpr index_type npos = container_type::npos;
    
    EntityGroupMap(const EntityGroup::RoleLimits& roleLimits,
                   uint32_t groupLimit,
                   const Allocator& allocator=Allocator());
    
        
    const container_type& rowset() const { return _rowset; }
    container_type& rowset() { return _rowset; }
    
    index_type rowIndexFromEntity(Entity eid) const;
    index_type allocateIndexForEntity(Entity eid);
    void removeDataFromEntity(Entity eid);
    
private:
    VectorDataRowset<EntityGroup> _rowset;
    unordered_map<Entity, uint32_t> _entityToGroupIndex;
};


} /* namespace overview */ } /* namespace cinek */

#endif /* defined(Overview_Entity_GroupMap_hpp) */
