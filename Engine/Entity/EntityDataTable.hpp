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
    using container_type = DataRowset;
    using index_type = typename container_type::index_type;
    static constexpr index_type npos = container_type::npos;
    
    EntityDataTable(const Descriptor& desc,
            uint32_t rowCount,
            const Allocator& allocator);
    
    const container_type& rowset() const { return _rowset; }
    container_type& rowset() { return _rowset; }
    
    index_type rowIndexFromEntity(Entity eid) const;
    index_type allocateIndexForEntity(Entity eid);
    void removeDataFromEntity(Entity eid);
    
private:
    Descriptor _descriptor;
    unordered_map<Entity, index_type> _entityToRow;
    container_type _rowset;
};

inline auto EntityDataTable::rowIndexFromEntity(Entity eid) const -> index_type
{
    auto indexIt = _entityToRow.find(eid);
    if (indexIt == _entityToRow.end())
        return npos;
    
    return indexIt->second;
}


template<typename Component, typename Container=EntityDataTable>
class Table
{
public:
    using value_type = Component;
    using container_type = Container;
    using index_type = typename Container::index_type;
    
    Table() : _table(nullptr) {}
    Table(container_type* dataTable) : _table(dataTable) {}
    Table(const Table& other) : _table(other._table) {}
    Table& operator=(const Table& other) { _table = other._table; return *this; }
    
    Table(Table&& other) : _table(other._table) { other._table = nullptr; }
    Table& operator=(Table&& other) { _table = other._table; other._table = nullptr; return *this; }
    
    operator bool() const { return _table != nullptr; }
    
    value_type* addDataToEntity(Entity eid);
    void removeDataFromEntity(Entity eid);

    const value_type* dataForEntity(Entity eid) const;
    value_type* dataForEntity(Entity eid);
    
    const container_type* dataTable() const { return _table; }
    container_type* dataTable() { return _table; }
    
    std::pair<Entity, value_type*> dataAtIndex(index_type index);
    std::pair<Entity, const value_type*> dataAtIndex(index_type index) const;
    
    //  iterate through all objects in the DB with the specified component
    //  Function signature should be:
    //      void fn(Entity eid, Component& component);
    template<typename Fn> Fn forEach(Fn fn);
    
private:
    container_type* _table;
};

template<typename Component, typename Container>
auto Table<Component, Container>::addDataToEntity(Entity eid) -> value_type*
{
    if (eid==0)
        return nullptr;
    
    auto index = _table->allocateIndexForEntity(eid);
    return reinterpret_cast<value_type*>(_table->rowset()[index]);
}

template<typename Component, typename Container>
auto Table<Component, Container>::dataForEntity(Entity eid) const -> const value_type*
{
    auto index = _table->rowIndexFromEntity(eid);
    if (index == Container::npos)
        return nullptr;
        
    return reinterpret_cast<const value_type*>(_table->rowset()[index]);
}

template<typename Component, typename Container>
auto Table<Component, Container>::dataForEntity(Entity eid) -> value_type*
{
    return const_cast<value_type*>(
        static_cast<const Table*>(this)->dataForEntity(eid)
    );
}

template<typename Component, typename Container>
auto Table<Component, Container>::dataAtIndex(index_type index)
    -> std::pair<Entity, value_type*>
{
    auto ret = static_cast<const Table*>(this)->dataAtIndex(index);
    return std::make_pair(ret.first, const_cast<value_type*>(ret.second));
}

template<typename Component, typename Container>
auto Table<Component, Container>::dataAtIndex(index_type index) const
    -> std::pair<Entity, const value_type*>
{
    auto& rowset = _table->rowset();
    return std::make_pair(rowset.entityAt(index), rowset.template at<value_type>(index));
}

//  iterate through all objects in the DB with the specified component
//  Function signature should be:
//      void fn(EntityId eid, Component& component);
template<typename Component, typename Container>
template<typename Fn>
Fn Table<Component, Container>::forEach(Fn fn)
{
    auto& rowset = _table->rowset();
    auto idx = rowset.firstIndex();
    while (idx != Container::npos)
    {
        fn(rowset.entityAt(idx), *rowset.template at<value_type>(idx));
        idx = rowset.nextIndex(idx);
    }
    return fn;
}

}

} /* namespace overview */ } /* namespace cinek */

#endif
