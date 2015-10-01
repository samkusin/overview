//
//  GfxTypes.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/26/15.
//
//

#include "GfxTypes.hpp"

#include "Texture.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Node.hpp"
#include "Model.hpp"

#include <cinek/debug.h>

#include <cinek/objectpool.inl>
#include <cinek/managed_handle.inl>

namespace cinek {

    //  necessary template impls for now - the ManagedObjectPool needs to have
    //  the definition of the object its pooling, and the managed handle needs
    //  the ManagedObjectPool implementation to compile and link.
    
    template class ManagedObjectPool<gfx::Texture, void>;
    template void ManagedHandle<gfx::Texture, ManagedObjectPool<gfx::Texture, void>>::acquire();
    template void ManagedHandle<gfx::Texture, ManagedObjectPool<gfx::Texture, void>>::release();
    
    template class ManagedObjectPool<gfx::Material, void>;
    template void ManagedHandle<gfx::Material, ManagedObjectPool<gfx::Material, void>>::acquire();
    template void ManagedHandle<gfx::Material, ManagedObjectPool<gfx::Material, void>>::release();
    
    template class ManagedObjectPool<gfx::Mesh, void>;
    template void ManagedHandle<gfx::Mesh, ManagedObjectPool<gfx::Mesh, void>>::acquire();
    template void ManagedHandle<gfx::Mesh, ManagedObjectPool<gfx::Mesh, void>>::release();
    
    template class ManagedObjectPool<gfx::Node, gfx::Model*>;
    template void ManagedHandle<gfx::Node, ManagedObjectPool<gfx::Node, gfx::Model*>>::acquire();
    template void ManagedHandle<gfx::Node, ManagedObjectPool<gfx::Node, gfx::Model*>>::release();

    namespace gfx {
    
        ////////////////////////////////////////////////////////////////////////////////
        /*
        typedef uint32_t NodeIndex;

        //  The index list is designed to minimize heap allocations for small lists.
        //  It accomplishes this by using 32-bit unsigned integers, with the requirement
        //  that index values do not exceed 2^31 (note the 31.) More on this reasoning
        //  later.
        //
        //  If the list capacity grows to beyond 4 items, the list uses allocated memory
        //  via the supplied allocator.
        //
        //  Internal footprint:
        //
        //  For small list:   4 Unsigned Ints with 32nd bit used to calculate size
        //                    This allows 4 bits to be used to calc size, but we only
        //                    need 3 for this impl - 2 bits for size (2^2=4), and 1 bit
        //                    to flag the list as static vs dynamic.  Bits 0-1 will be
        //                    used for size.  Bit 2 ignored, and Bit 3 == 1 signals this
        //                    is going to be a dynamic list.
        //
        //  For dynamic list: Our 32 byte (4 uint32_t) buffer will contain the following
        //                    elements - bytes 0-7 are the 32/64-bit pointer to the buffer
        //                    (yes, x64 systems don't use all the bits, but since we've
        //                     another 8 bytes or storage to use for other elements,
        //                     we're not going to worry about attempting even more
        //                     optimizations.)
        //
        //                    Bytes 8-11 are used as our array size, and 12-15 are used
        //                    for capacity.   Note bit 31 for capacity will be on to
        //                    indicate this is a dynamic list - but we don't support
        //                    list sizes beyond 2^31 elements, and shouldn't ever try
        //                    to do so with this engine.
        //
        class NodeElementIndexList
        {
            CK_CLASS_NON_COPYABLE(NodeElementIndexList);
            
        public:
            NodeElementIndexList();
            ~NodeElementIndexList();
            NodeElementIndexList(NodeElementIndexList&& other);
            NodeElementIndexList& operator=(NodeElementIndexList&& other);
            
            uint32_t size() const;
            uint32_t capacity() const;
            
            NodeIndex operator[](uint32_t index) const;
            NodeIndex at(uint32_t index) const;
            void set(uint32_t index, NodeIndex v);
            
            void add(NodeIndex v);
            void insert(uint32_t index, NodeIndex v);
            void erase(uint32_t index);
            
            void clear();
            void reserve(uint32_t size);
            
        private:
            enum { kSmallArraySize = 4 };
            // TODO - use std::max instead of static_assert when we support C++14
            // on a 32-bit system, our dynamic buffer only needs 3 uint32_t values,
            // though we sill store 4 uint32_ts.
            uint8_t _data[sizeof(uint32_t)*kSmallArraySize];
            static_assert(sizeof(uint32_t*) <= sizeof(uint32_t)*2,
                          "Supports only 32-bit or 64-bit systems/pointers");
            static_assert(sizeof(_data) >= sizeof(uint32_t*)+sizeof(uint32_t)*2,
                          "Buffer must contain enough room for dynamic list");
            
            void freeBuffer();
            void allocateBuffer(uint32_t size);
            void reserveBuffer(uint32_t capacity);
            bool isDynamic() const;
            void moveFrom(NodeElementIndexList& other);
        };

        static const uint32_t kNodeIndexMask = 0xefffffff;
        static const uint32_t kNodeIndexHighBit = 0x80000000;
    
        static inline uint32_t staticNodeElementIndexListSize(const uint32_t* elems) {
            return ((elems[1]&kNodeIndexHighBit) >> 30) | ((elems[0]&kNodeIndexHighBit)>>31);
        }

        NodeElementIndexList::NodeElementIndexList()
        {
            uint32_t* elems = reinterpret_cast<uint32_t*>(_data);
            elems[0] = elems[1] = 0;
            elems[kSmallArraySize-1] = 0;
        }

        NodeElementIndexList::~NodeElementIndexList()
        {
            freeBuffer();
        }
        
        NodeElementIndexList::NodeElementIndexList(NodeElementIndexList&& other)
        {
            moveFrom(other);
        }
        
        NodeElementIndexList& NodeElementIndexList::operator=(NodeElementIndexList&& other)
        {
            moveFrom(other);
            return *this;
        }
        
        void NodeElementIndexList::moveFrom(cinek::gfx::NodeElementIndexList &other)
        {
            uint32_t* elems = reinterpret_cast<uint32_t*>(_data);
            uint32_t* otherElems = reinterpret_cast<uint32_t*>(other._data);
            for (int i = 0; i < kSmallArraySize; ++i) {
                elems[i] = otherElems[i];
                otherElems[i] = 0;
            }
        }
    
        void NodeElementIndexList::allocateBuffer(uint32_t size)
        {
            if (size < capacity())
                return;
            uint32_t newCapacity = (capacity() * 2) & kNodeIndexMask;
            
            reserveBuffer(newCapacity);
        }
        
        void NodeElementIndexList::reserveBuffer(uint32_t newCapacity)
        {
            CK_ASSERT_RETURN(newCapacity != capacity());
            
            uint32_t* buffer = new uint32_t[newCapacity];
            CK_ASSERT_RETURN(buffer);

            uint32_t* elems = reinterpret_cast<uint32_t*>(_data);
            uint32_t size = 0;
            if (!isDynamic()) {
                //  remove metadata from the static list since we're moving elements
                //  to our dynamic buffer
                size = staticNodeElementIndexListSize(elems);
                elems[0] &= kNodeIndexMask;
                elems[1] &= kNodeIndexMask;
                memcpy(buffer, elems, sizeof(elems[0]) * kSmallArraySize);
            }
            else  {
                size = elems[2];
                uint32_t* values = *reinterpret_cast<uint32_t**>(_data);
                if (size > 0) {
                    memcpy(buffer, values, size * sizeof(uint32_t));
                }
                delete[] values;
            }
            
            //  set our buffer attributes and flag the dynamic buffer bit
            *reinterpret_cast<uint32_t**>(elems) = buffer;
            elems[2] = size;
            elems[3] = newCapacity;
            elems[3] |= kNodeIndexHighBit;
        }

        void NodeElementIndexList::freeBuffer()
        {
            if (isDynamic()) {
                uint32_t* buf = *reinterpret_cast<uint32_t**>(_data);
                delete [] buf;
            }
        }
        
        bool NodeElementIndexList::isDynamic() const
        {
            const uint32_t* elems = reinterpret_cast<const uint32_t*>(_data);
            return (elems[3] & kNodeIndexHighBit) != 0;
        }

        uint32_t NodeElementIndexList::size() const
        {
            const uint32_t* elems = reinterpret_cast<const uint32_t*>(_data);
            if (isDynamic()) {
                return elems[2];
            }
            else {
                return staticNodeElementIndexListSize(elems);
            }
        }

        uint32_t NodeElementIndexList::capacity() const
        {
            const uint32_t* elems = reinterpret_cast<const uint32_t*>(_data);
            if (isDynamic()) {
                return elems[3] & kNodeIndexMask;
            }
            else {
                return kSmallArraySize;
            }
        }

        NodeIndex NodeElementIndexList::operator[](uint32_t index) const
        {
            return at(index);
        }

        NodeIndex NodeElementIndexList::at(uint32_t index) const
        {
            const uint32_t* elems = reinterpret_cast<const uint32_t*>(_data);
            if (isDynamic()) {
                CK_ASSERT_RETURN_VALUE(index < elems[2], 0);
                const uint32_t* p = *reinterpret_cast<const uint32_t**>((uintptr_t)_data);
                return p[index];
            }
            else {
                CK_ASSERT_RETURN_VALUE(index < staticNodeElementIndexListSize(elems), 0);
                return elems[index] & kNodeIndexMask;
            }
        }
        
        void NodeElementIndexList::set(uint32_t index, NodeIndex v)
        {
            uint32_t* elems = reinterpret_cast<uint32_t*>(_data);
            if (isDynamic()) {
                CK_ASSERT_RETURN(index < elems[2]);
                elems = *reinterpret_cast<uint32_t**>((uintptr_t)_data);
            }
            else {
                CK_ASSERT_RETURN(index < staticNodeElementIndexListSize(elems));
            }
            
            elems[index] = v & kNodeIndexMask;
        }

        void NodeElementIndexList::add(NodeIndex v)
        {
            insert(size(), v);
        }
        
        void NodeElementIndexList::insert(uint32_t index, NodeIndex v)
        {
            uint32_t* elems = reinterpret_cast<uint32_t*>(_data);
            uint32_t sz = size();
            CK_ASSERT_RETURN(index <= sz);
            if (!isDynamic()) {
                if (sz < kSmallArraySize) {
                    for (uint32_t i = sz; i > index; --index)
                        elems[i] = elems[i-1] & kNodeIndexMask;
                    ++sz;
                    elems[index] = v & kNodeIndexMask;
                    elems[1] &= kNodeIndexMask;     // reset size bits
                    elems[0] &= kNodeIndexMask;
                    elems[1] |= ((sz & 0x2)<<30);
                    elems[0] |= ((sz & 0x1)<<31);
                    return;
                }
            }
            ++sz;
            allocateBuffer(sz);
            uint32_t limit = capacity();
            if (isDynamic() && sz <= limit) {
                uint32_t* values = *reinterpret_cast<uint32_t**>(_data);
                for (uint32_t i = sz; i > index; --index)
                    values[i] = values[i-1];
                values[index] = v & kNodeIndexMask;
                elems[2] = sz;
            }
        }
        
        void NodeElementIndexList::erase(uint32_t index)
        {
            CK_ASSERT_RETURN(index < size());
            uint32_t* elems = reinterpret_cast<uint32_t*>(_data);
            uint32_t sz = size();
            if (isDynamic()) {
                uint32_t* values = *reinterpret_cast<uint32_t**>(_data);
                for (uint32_t i = index; i < sz-1; ++i)
                    values[i] = values[i+1];
                --elems[2];
            }
            else {
                for (uint32_t i = index; i < sz-1; ++i)
                    elems[i] = elems[i+1] & kNodeIndexMask;
                --sz;
                elems[1] &= kNodeIndexMask;     // reset size bits
                elems[0] &= kNodeIndexMask;
                elems[1] |= ((sz & 0x2)<<30);
                elems[0] |= ((sz & 0x1)<<31);
            }
        }

        void NodeElementIndexList::clear()
        {
            uint32_t* elems = reinterpret_cast<uint32_t*>(_data);
            if (isDynamic()) {
                elems[2] = 0;
            }
            else {
                elems[1] &= kNodeIndexMask;     // clear size bits
                elems[0] &= kNodeIndexMask;
            }
        }
        
        void NodeElementIndexList::reserve(uint32_t size)
        {
            if (!isDynamic()) {
                if (size > kSmallArraySize) {
                    allocateBuffer(size);
                }
                return;
            }
            
            if (size > capacity()) {
                reserveBuffer(size);
            }
        }
        */
    }   //  namespace gfx
}   //  namespace cinek
