//
//  TransformDataContext.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 3/2/16.
//
//

#include "TransformDataContext.hpp"
#include "TransformBody.hpp"

#include <cinek/objectpool.inl>

namespace cinek {

template class ObjectPool<ove::TransformAction>;
template class ObjectPool<ove::TransformSequence>;
template class ObjectPool<ove::TransformBody>;

TransformDataContext::TransformDataContext(InitParams params) :
    _sequencePool(params.sequenceCount),
    _actionPool(params.actionCount),
    _setPool(params.setCount),
    _setDict(),
    _bodyPool(params.bodyCount)
{
}

ove::TransformSequence* TransformDataContext::allocateSequence()
{
    ove::TransformSequence* seq = _sequencePool.construct();
    return seq;
}

void TransformDataContext::freeSequence(ove::TransformSequence* seq)
{
    _sequencePool.destruct(seq);
}

ove::TransformAction* TransformDataContext::allocateAction()
{
    ove::TransformAction* container = _actionPool.construct();
    return container;
}

void TransformDataContext::freeAction(ove::TransformAction* container)
{
    _actionPool.destruct(container);
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

ove::TransformBody* TransformDataContext::allocateBody
(
    Entity entity,
    ove::TransformSetHandle setHandle
)
{
    return _bodyPool.construct(entity, setHandle);
}

void TransformDataContext::freeBody(ove::TransformBody* body)
{
    _bodyPool.destruct(body);
}
    
    
} /* namespace cinek */