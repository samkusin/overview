//
//  DataRowset.cpp
//  Overview
//
//  Created by Samir Sinha on 5/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "DataRowset.hpp"
#include "Engine/Debug.hpp"

namespace cinek { namespace overview {

namespace component
{
    DataRowset::DataRowset
    (
        const Descriptor& desc,
        uint32_t rowCount,
        const Allocator& allocator
    ) :
        _allocator(allocator),
        _header(desc),
        _rowstart(nullptr),
        _rowend(nullptr),
        _rowlimit(nullptr),
        _freestart(nullptr),
        _freeend(nullptr),
        _freelimit(nullptr)
    
    {
        CK_ASSERT(rowCount != npos);
        
        _header.recordSize += sizeof(EntityId); // flags included in record
        _rowstart = (uint8_t*)_allocator.alloc(_header.recordSize * rowCount);
        _rowend = _rowstart;
        _rowlimit = _rowstart + _header.recordSize*rowCount;
        _freestart = (uint32_t*)_allocator.alloc(sizeof(uint32_t)*rowCount);
        _freeend = _freestart;
        _freelimit = _freestart + rowCount;
    }
    
    DataRowset::~DataRowset()
    {
        _allocator.free(_freestart);
        _freestart = _freeend = _freelimit = nullptr;
        
        _allocator.free(_rowstart);
        _rowstart = _rowend = _rowlimit = nullptr;
    }
        
    DataRowset::DataRowset(DataRowset&& other) :
        _allocator(std::move(other._allocator)),
        _header(std::move(other._header)),
        _rowstart(other._rowstart),
        _rowend(other._rowend),
        _rowlimit(other._rowlimit),
        _freestart(other._freestart),
        _freeend(other._freeend),
        _freelimit(other._freelimit)
    {
        other._header.recordSize = 0;
        other._header.id = kEmpty;
        other._rowstart = nullptr;
        other._rowend = nullptr;
        other._rowlimit = nullptr;
        other._freestart = nullptr;
        other._freeend = nullptr;
        other._freelimit = nullptr;
    }
    
    DataRowset& DataRowset::operator=(DataRowset&& other)
    {
        _allocator = std::move(other._allocator);
        _header = std::move(other._header);
        _rowstart = other._rowstart;
        _rowend = other._rowend;
        _rowlimit = other._rowlimit;
        _freestart = other._freestart;
        _freeend = other._freeend;
        _freelimit = other._freelimit;
        
        other._header.recordSize = 0;
        other._header.id = kEmpty;
        other._rowstart = nullptr;
        other._rowend = nullptr;
        other._rowlimit = nullptr;
        other._freestart = nullptr;
        other._freeend = nullptr;
        other._freelimit = nullptr;
        
        return *this;
    }
    
    EntityId* DataRowset::rowAt(index_type index)
    {
        return const_cast<EntityId*>(static_cast<const DataRowset*>(this)->rowAt(index));
    }
    
    const EntityId* DataRowset::rowAt(index_type index) const
    {
        return reinterpret_cast<const EntityId*>(_rowstart + index*_header.recordSize);
    }
        
    auto DataRowset::allocate(EntityId eid) -> index_type
    {
        index_type idx = npos;
        //  remove from free list first
        if (_freestart != _freeend)
        {
            --_freeend;
            idx = *_freeend;
        }
        else if (_rowend != _rowlimit)
        {
            idx = (index_type)(_rowend - _rowstart) / _header.recordSize;
            _rowend += _header.recordSize;
        }
        if (idx != npos)
        {
            EntityId* p = rowAt(idx);
            *p = eid;
            memset(p+1, 0, _header.recordSize-sizeof(uint32_t));
        }
        return idx;
    }
    
    void DataRowset::free(index_type index)
    {
        if (index >= size())
            return;
        EntityId* p = rowAt(index);
        if (p[0])
        {
            p[0] = kNullEntity;
            CK_ASSERT(_freeend != _freelimit);
            if (_freeend != _freelimit)
            {
                *_freeend = index;
                ++_freeend;
            }
        }
    }
        
    uint32_t DataRowset::size() const
    {
        return (uint32_t)(_rowend - _rowstart) / _header.recordSize;
    }
    
    uint32_t DataRowset::capacity() const
    {
        return (uint32_t)(_rowlimit - _rowstart) / _header.recordSize;
    }
        
    uint8_t* DataRowset::operator[](index_type index)
    {
        return at(index);
    }
    
    const uint8_t* DataRowset::operator[](index_type index) const
    {
        return at(index);
    }
        
    uint8_t* DataRowset::at(index_type index)
    {
        return const_cast<uint8_t*>(static_cast<const DataRowset*>(this)->at(index));
    }
    
    const uint8_t* DataRowset::at(index_type index) const
    {
        CK_ASSERT(index < size() && index != npos);
        
        const EntityId* p = rowAt(index);
        return p[0] ? reinterpret_cast<const uint8_t*>(p+1) : nullptr;
    }
    
    EntityId DataRowset::entityAt(index_type index) const
    {
        const EntityId* p = rowAt(index);
        return p[0];
    }
    
    auto DataRowset::firstIndex(index_type idx) const -> index_type
    {
        //  return the first valid component data row (might not be the first
        //  row, for instance.)
        if (idx != npos)
        {
            if (_rowstart != _rowend)
            {
                const EntityId* row = rowAt(0);
                if (row[0])
                    return 0;
                
                return nextIndex(idx);
            }
        }
        return npos;
    }
        
    auto DataRowset::nextIndex(index_type idx) const -> index_type
    {
        if (idx != npos)
        {
            index_type sz = size();
            //  indices are unsigned - so take care of the sz==0 edge case.
            //  this will allow us to iterate idx without overflow concerns
            if (!sz)
                return npos;
            
            while (idx < sz-1)
            {
                ++idx;
                const EntityId* row = rowAt(idx);
                if (row[0])
                    return idx;
            }
        }
        return npos;
    }
    
    auto DataRowset::prevIndex(index_type idx) const -> index_type
    {
        if (idx != npos)
        {
            //  handle edge cases
            index_type sz = size();
            if (!idx || idx >= sz)
                return npos;
  
            while (idx > 0)
            {
                --idx;
                const EntityId* row = rowAt(idx);
                if (row[0])
                    return idx;
            }
        }
        return npos;
    }
    
}

} /* namespace overview */ } /* namespace cinek */
