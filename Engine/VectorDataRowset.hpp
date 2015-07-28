//
//  VectorDataRowset.hpp
//  Overview
//
//  Created by Samir Sinha on 7/27/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_VectorDataRowset_hpp
#define Overview_VectorDataRowset_hpp

#include <cinek/debug.hpp>
#include <cinek/vector.hpp>

namespace cinek { namespace overview {

template<typename _Data>
class VectorDataRowset
{
public:
    using data_type = _Data;
    using index_type = ComponentRowIndex;
    static const index_type npos = kNullComponentRow;
    
    VectorDataRowset(const Allocator& allocator=Allocator());
    VectorDataRowset(uint32_t sz, const Allocator& allocator=Allocator());
    template<typename DataInitParam0>
    VectorDataRowset(uint32_t sz, const DataInitParam0& initParam,
                     const Allocator& allocator=Allocator());
    
    VectorDataRowset(VectorDataRowset&& other);
    VectorDataRowset& operator=(VectorDataRowset&& other);
        
    index_type allocate(Entity eid);
    void free(index_type index);
        
    uint32_t size() const;
    uint32_t capacity() const;
    
    template<typename Component=_Data> data_type* at(index_type index);
    template<typename Component=_Data> const data_type* at(index_type index) const;
    
    Entity entityAt(index_type index) const;
        
    index_type firstIndex(index_type idx=0) const;
    index_type nextIndex(index_type) const;
    index_type prevIndex(index_type) const;
    
private:
    vector<std::pair<Entity, data_type>> _data;
    vector<uint32_t> _freelist;
    uint32_t _highestGroupIndexAllocated;

    void resetData(data_type& data);
};

template<typename _Data>
VectorDataRowset<_Data>::VectorDataRowset(const Allocator& allocator) :
    _data(allocator),
    _freelist(allocator),
    _highestGroupIndexAllocated(0)
{
}

template<typename _Data>
VectorDataRowset<_Data>::VectorDataRowset
(
    uint32_t sz,
    const Allocator& allocator
) :
    VectorDataRowset(allocator)
{
    //  preallocate all memory required for the map
    _data.reserve(sz);
    _freelist.reserve(sz);
    
    for (uint32_t i = 0; i < sz; ++i)
    {
        _data.emplace_back(Entity::null(), data_type());
    }
}


template<typename _Data>
template<typename DataInitParam0>
VectorDataRowset<_Data>::VectorDataRowset
(
    uint32_t sz,
    const DataInitParam0& initParam,
    const Allocator& allocator
) :
    VectorDataRowset(allocator)
{
    //  preallocate all memory required for the map
    _data.reserve(sz);
    _freelist.reserve(sz);
    
    for (uint32_t i = 0; i < sz; ++i)
    {
        data_type data(initParam, allocator);
        _data.emplace_back(Entity::null(), std::move(data));
    }
}

template<typename _Data>
VectorDataRowset<_Data>::VectorDataRowset(VectorDataRowset&& other) :
    _data(std::move(other._groups)),
    _freelist(std::move(other._freelist)),
    _highestGroupIndexAllocated(other._highestGroupIndexAllocated)
{
    other._highestGroupIndexAllocated = 0;
}

template<typename _Data>
VectorDataRowset<_Data>& VectorDataRowset<_Data>::operator=(VectorDataRowset&& other)
{
    _data = std::move(other._groups);
    _freelist = std::move(other._freelist);
    _highestGroupIndexAllocated = other._highestGroupIndexAllocator;
    other._highestGroupIndexAllocated = 0;
    return *this;
}

template<typename _Data>
void VectorDataRowset<_Data>::resetData(data_type& data)
{
    data.reset();
}
    
template<typename _Data>
auto VectorDataRowset<_Data>::allocate(Entity eid) -> index_type
{
    uint32_t idx;

    if (!_freelist.empty())
    {
        idx = _freelist.back();
        _freelist.pop_back();
    }
    else if (_highestGroupIndexAllocated < capacity())
    {
        idx = _highestGroupIndexAllocated++;
    }
    else
    {
        return npos;
    }
    
    resetData(_data[idx].second);
    _data[idx].first = eid;
    return idx;
}

template<typename _Data>
void VectorDataRowset<_Data>::free(index_type index)
{
    if (index == npos)
        return;
    
    CK_ASSERT_RETURN(index < _data.size() && _data[index].first.valid());
    
    _data[index].first = nullptr;
    
    _freelist.push_back(index);
}
  
template<typename _Data>
uint32_t VectorDataRowset<_Data>::size() const
{
    return _highestGroupIndexAllocated;
}

template<typename _Data>
uint32_t VectorDataRowset<_Data>::capacity() const
{
    return (uint32_t)_data.size();
}

template<typename _Data>
template<typename Component>
auto VectorDataRowset<_Data>::at(index_type index) -> data_type*
{
    return const_cast<data_type*>(static_cast<const VectorDataRowset*>(this)->at(index));
}

template<typename _Data>
template<typename Component>
auto VectorDataRowset<_Data>::at(index_type index) const -> const data_type*
{
    CK_ASSERT_RETURN_VALUE(index < size(), nullptr);
    CK_ASSERT_RETURN_VALUE(_data[index].first.valid(), nullptr);
    return &_data[index].second;
}

template<typename _Data>
Entity VectorDataRowset<_Data>::entityAt(index_type index) const
{
    CK_ASSERT_RETURN_VALUE(index < size(), Entity::null());
    return _data[index].first;
}

template<typename _Data>
auto VectorDataRowset<_Data>::firstIndex(index_type idx) const -> index_type
{
    while (idx < size())
    {
        if (_data[idx].first.valid())
            return idx;
    }
    return npos;
}

template<typename _Data>
auto VectorDataRowset<_Data>::nextIndex(index_type idx) const -> index_type
{
    while (idx < size())
    {
        if (_data[idx].first.valid())
            return idx;
    }
    return npos;
}

template<typename _Data>
auto VectorDataRowset<_Data>::prevIndex(index_type idx) const -> index_type
{
    if (idx > size())
        return npos;
    
    while (idx > 0)
    {
        --idx;
        if (_data[idx].first.valid())
            return idx;
    }
    return npos;
}

} /* namespace overview */ } /* namespace cinek */

#endif
