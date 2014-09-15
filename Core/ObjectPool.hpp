//
//  ObjectPool.hpp
//  Overview
//
//  Created by Samir Sinha on 8/18/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Core_ObjectPool_hpp
#define CK_Core_ObjectPool_hpp

#include "cinek/allocator.hpp"
#include "cinek/memoryregion.hpp"

namespace cinekine {

    template<class _T>
    class ObjectPool
    {
        CK_CLASS_NON_COPYABLE(ObjectPool);

    public:
        typedef _T          value_type;
        typedef _T*         pointer;

        ObjectPool(size_t objectCount, const Allocator& allocator);

        template<class... Args> pointer construct(Args&&... args);
        void destruct(pointer item);

    private:
        MemoryRegion<sizeof(_T)> _region;
    };

    template<class _T>
    ObjectPool<_T>::ObjectPool(size_t objectCount, const Allocator& allocator) :
        _region(objectCount, allocator)
    {
    }

    template<typename _T> template<class... Args>
    auto ObjectPool<_T>::construct(Args&&... args) -> pointer
    {
        pointer ret = reinterpret_cast<pointer>(_region.allocate());
        if (ret)
        {
            ::new((void*)ret) value_type(std::forward<Args>(args)...);
        }
        return ret;
    }

    template<typename _T>
    void ObjectPool<_T>::destruct(pointer item)
    {
        item->~value_type();
        _region.deallocate(item);
    }


}   // namespace cinekine


#endif
