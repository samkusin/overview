//
//  TransformDataContext.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 3/2/16.
//
//

#include "TransformDataContext.hpp"

#include <cinek/objectpool.inl>

namespace cinek {

template class ObjectPool<ove::TransformContainer>;
template class ObjectPool<ove::TransformSequence>;
    
TransformDataContext::TransformDataContext(InitParams params) :
    _sequencePool(params.sequenceCount),
    _containerPool(params.containerCount),
    _setPool(params.setCount),
    _setDict()
{
}
    
} /* namespace cinek */