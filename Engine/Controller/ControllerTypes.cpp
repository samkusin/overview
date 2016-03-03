//
//  ControllerTypes.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 3/1/16.
//
//

#include "ControllerTypes.hpp"

#include "TransformSet.hpp"

#include <cinek/objectpool.inl>
#include <cinek/managed_handle.inl>

namespace cinek {

    template class ObjectPool<ove::TransformSet>;
    template class ManagedObjectPool<ove::TransformSet, void>;
    template void ManagedHandle<ove::TransformSet, ManagedObjectPool<ove::TransformSet, void>>::acquire();
    template void ManagedHandle<ove::TransformSet, ManagedObjectPool<ove::TransformSet, void>>::release();
    template class ManagedObjectPoolBase<ove::TransformSet, ManagedObjectPool<ove::TransformSet, void>>;

    namespace ove {
    
    } /* namespace ove */
    
} /* namespace cinek */
