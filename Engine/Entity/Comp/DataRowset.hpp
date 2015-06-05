//
//  DataRowset.hpp
//  Overview
//
//  Created by Samir Sinha on 5/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Component_DataRowset_hpp
#define Overview_Entity_Component_DataRowset_hpp

#include "Engine/Entity/EntityTypes.hpp"

#include <cinek/allocator.hpp>
#include <cinek/types.hpp>

#include <iterator>

namespace cinek { namespace overview {

namespace component
{
    class DataRowset
    {
        CK_CLASS_NON_COPYABLE(DataRowset);
        
    public:
        using index_type = ComponentRowIndex;
        
        static const index_type npos = kNullComponentRow;

        DataRowset(const Descriptor& desc,
            uint32_t rowCount,
            const Allocator& allocator=Allocator());
        ~DataRowset();
        
        DataRowset(DataRowset&& other);
        DataRowset& operator=(DataRowset&& other);
        
        index_type allocate(EntityId eid);
        void free(index_type index);
        
        uint32_t size() const;
        uint32_t capacity() const;
        
        uint8_t* operator[](index_type index);
        const uint8_t* operator[](index_type index) const;
        
        template<typename Component> Component* at(index_type index);
        template<typename Component> const Component* at(index_type index) const;
        
        uint8_t* at(index_type index);
        const uint8_t* at(index_type index) const;
        
        EntityId entityAt(index_type index) const;
        
        index_type firstIndex(index_type idx=0) const;
        index_type nextIndex(index_type) const;
        index_type prevIndex(index_type) const;
        
    private:
        Allocator _allocator;
        Descriptor _header;
        
        // rows
        //  each row contains a EntityId header + data[header.recordSize]
        //  the entity id will be zero if the row is not used
        uint8_t* _rowstart;
        uint8_t* _rowend;
        uint8_t* _rowlimit;
        // free list
        uint32_t* _freestart;
        uint32_t* _freeend;
        uint32_t* _freelimit;
        
        EntityId* rowAt(index_type index);
        const EntityId* rowAt(index_type index) const;
    };

    template<typename Component>
    Component* DataRowset::at(index_type index)
    {
        if (_header.id != Component::kComponentId)
            return nullptr;
        
        return reinterpret_cast<Component*>(at(index));
    }

    template<typename Component>
    const Component* DataRowset::at(index_type index) const
    {
        if (_header.id != Component::kComponentId)
            return nullptr;
        
        return reinterpret_cast<const Component*>(at(index));
    }
}



} /* namespace overview */ } /* namespace cinek */

#endif /* defined(__Overview__DataRowset__) */
