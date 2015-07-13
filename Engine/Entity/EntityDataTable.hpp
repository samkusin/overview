//
//  EntityDataTable.hpp
//  Overview
//
//  Created by Samir Sinha on 6/19/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_ComponentTable_hpp
#define Overview_Entity_ComponentTable_hpp

#include "EntityTypes.hpp"
#include "DataRowset.hpp"

#include <cinek/map.hpp>

namespace cinek { namespace overview {

namespace component
{

class EntityDataTable
{
public:
    EntityDataTable(const Descriptor& desc,
            uint32_t rowCount,
            const Allocator& allocator);
    
    const DataRowset& rowset() const { return _rowset; }
    DataRowset& rowset() { return _rowset; }
    
    DataRowset::index_type rowIndexFromEntity(Entity eid) const;
    DataRowset::index_type allocateIndexForEntity(Entity eid);
    void removeComponentFromEntity(Entity eid);
    
private:
    Descriptor _descriptor;
    unordered_map<Entity, DataRowset::index_type> _entityToRow;
    DataRowset _rowset;
};

inline DataRowset::index_type EntityDataTable::rowIndexFromEntity(Entity eid) const
{
    auto indexIt = _entityToRow.find(eid);
    if (indexIt == _entityToRow.end())
        return DataRowset::npos;
    
    return indexIt->second;
}


template<typename Component>
class Table
{
public:
    using value_type = Component;
    using index_type = DataRowset::index_type;
    
    Table() : _table(nullptr) {}
    Table(EntityDataTable* dataTable) : _table(dataTable) {}
    Table(const Table& other) : _table(other._table) {}
    Table& operator=(const Table& other) { _table = other._table; return *this; }
    
    Table(Table&& other) : _table(other._table) { other._table = nullptr; }
    Table& operator=(Table&& other) { _table = other._table; other._table = nullptr; return *this; }
    
    operator bool() const { return _table != nullptr; }
    
    Component* addComponentToEntity(Entity eid);
    void removeComponentFromEntity(Entity eid);

    const Component* rowForEntity(Entity eid) const;
    Component* rowForEntity(Entity eid);
    
    const EntityDataTable* data() const { return _table; }
    EntityDataTable* data() { return _table; }
    
    std::pair<Entity, Component*> rowAtIndex(index_type index);
    std::pair<Entity, const Component*> rowAtIndex(index_type index) const;
    
    //  iterate through all objects in the DB with the specified component
    //  Function signature should be:
    //      void fn(Entity eid, Component& component);
    template<typename Fn> Fn forEach(Fn fn);
    
private:
    EntityDataTable* _table;
};

template<typename Component>
Component* Table<Component>::addComponentToEntity(Entity eid)
{
    if (eid==0)
        return nullptr;
    
    auto index = _table->allocateIndexForEntity(eid);
    return reinterpret_cast<Component*>(_table->rowset()[index]);
}

template<typename Component>
const Component* Table<Component>::rowForEntity(Entity eid) const
{
    auto index = _table->rowIndexFromEntity(eid);
    if (index == DataRowset::npos)
        return nullptr;
        
    return reinterpret_cast<const Component*>(_table->rowset()[index]);
}

template<typename Component>
Component* Table<Component>::rowForEntity(Entity eid)
{
    return const_cast<Component*>(
        static_cast<const Table*>(this)->rowForEntity(eid)
    );
}

template<typename Component>
std::pair<Entity, Component*> Table<Component>::rowAtIndex(index_type index)
{
    auto ret = static_cast<const Table*>(this)->rowAtIndex(index);
    return std::make_pair(ret.first, const_cast<Component*>(ret.second));
}

template<typename Component>
std::pair<Entity, const Component*> Table<Component>::rowAtIndex(index_type index) const
{
    auto& rowset = _table->rowset();
    return std::make_pair(rowset.entityAt(index), rowset.at<Component>(index));
}

//  iterate through all objects in the DB with the specified component
//  Function signature should be:
//      void fn(EntityId eid, Component& component);
template<typename Component>
template<typename Fn>
Fn Table<Component>::forEach(Fn fn)
{
    auto& rowset = _table->rowset();
    auto idx = rowset.firstIndex();
    while (idx != component::DataRowset::npos)
    {
        fn(rowset.entityAt(idx), *rowset.template at<Component>(idx));
        idx = rowset.nextIndex(idx);
    }
    return fn;
}

}

} /* namespace overview */ } /* namespace cinek */

#endif
