//
//  EntityGroupMap.hpp
//  Overview
//
//  Created by Samir Sinha on 7/25/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_GroupMap_hpp
#define Overview_Entity_GroupMap_hpp

#include "Engine/Entity/EntityGroup.hpp"
#include "Engine/Entity/EntityDataTable.hpp"
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
    using id_type = EntityGroupMapId;
    static constexpr index_type npos = container_type::npos;
    
    struct MakeDescriptor
    {
        id_type id;
        EntityGroup::RoleLimits roleLimits;
        uint32_t cnt;
    };
    
    EntityGroupMap(const MakeDescriptor& makeDesc,
                   const Allocator& allocator=Allocator());
    
    
    id_type id() const { return _id; }
    
    const container_type& rowset() const { return _rowset; }
    container_type& rowset() { return _rowset; }
    
    index_type rowIndexFromEntity(Entity eid) const;
    index_type allocateIndexForEntity(Entity eid);
    void removeDataFromEntity(Entity eid);
    
    index_type usedCount() const { return (index_type)_entityToGroupIndex.size(); }
    
private:
    VectorDataRowset<EntityGroup> _rowset;
    unordered_map<Entity, uint32_t> _entityToGroupIndex;
    id_type _id;
};

namespace component
{

using EntityGroupTable = Table<EntityGroup, EntityGroupMap>;

template<>
Table<EntityGroup, EntityGroupMap>::Table(EntityGroupMap*);

}


} /* namespace overview */ } /* namespace cinek */

#endif /* defined(Overview_Entity_GroupMap_hpp) */
