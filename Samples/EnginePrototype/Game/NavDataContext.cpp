//
//  NavDatContext.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/17/16.
//
//

#include "NavDataContext.hpp"
#include "Engine/Controller/NavBody.hpp"
#include "Engine/Game/NavSceneBodyTransform.hpp"

#include <cinek/objectpool.inl>

namespace cinek {

template class ObjectPool<ove::NavBody>;
template class ObjectPool<ove::NavSceneBodyTransform>;

NavDataContext::NavDataContext(const InitParams& initParams) :
    _navBodyPool(initParams.navBodyCount),
    _navSceneBodyPool(initParams.navBodyCount)
{
}

    
ove::NavBody* NavDataContext::allocateBody(const ove::NavBody::InitProperties& props)
{
    return _navBodyPool.construct(props);
}

ove::NavSceneBodyTransform* NavDataContext::allocateTransform
(
    ove::SceneBody* sceneBody,
    Entity entity
)
{
    auto inserter = _transformMap.emplace(entity, nullptr);
    if (inserter.first != _transformMap.end()) {
        ove::NavSceneBodyTransform* transform = _navSceneBodyPool.construct(sceneBody);
        (*inserter.first).second = transform;
    }
    return (*inserter.first).second;
}

ove::NavBody* NavDataContext::cloneBody
(
    const ove::NavBody* source,
    ove::SceneBody* sceneBody,
    Entity entity
)
{
    ove::NavBody* body = _navBodyPool.construct(*source, entity);
    if (body) {
        ove::NavSceneBodyTransform* transform = allocateTransform(sceneBody, entity);
        body->setTransform(transform);
    }
    return body;
}

void NavDataContext::freeBody(ove::NavBody* body)
{
    auto it = _transformMap.find(body->entity());
    if (it != _transformMap.end()) {
        _navSceneBodyPool.destruct(it->second);
    }
    _navBodyPool.destruct(body);
}
    
} /* namespace cinek */