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

ove::TransformSequence* TransformDataContext::createSequence()
{
    ove::TransformSequence* seq = _sequencePool.construct();
    return seq;
}

void TransformDataContext::destroySequence(ove::TransformSequence* seq)
{
    _sequencePool.destruct(seq);
}

ove::TransformContainer* TransformDataContext::createContainer()
{
    ove::TransformContainer* container = _containerPool.construct();
    return container;
}

void TransformDataContext::destroyContainer(ove::TransformContainer* container)
{
    _containerPool.destruct(container);
}

ove::TransformSetHandle TransformDataContext::registerSet
(
    ove::TransformSet&& set,
    std::string name
)
{
    return registerResource(std::move(set), _setPool, _setDict, name.c_str());
}

ove::TransformSetHandle TransformDataContext::findSet(const std::string& name) const
{
    auto it = _setDict.find(name);
    if (it == _setDict.end())
        return nullptr;
    
    return it->second;
}
    
    
} /* namespace cinek */