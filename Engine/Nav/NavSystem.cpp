//
//  NavSystem.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/17/16.
//
//

#include "NavSystem.hpp"

#include "Engine/Nav/NavBody.hpp"
#include "Engine/Debug.hpp"

#include <algorithm>

namespace cinek {
    namespace ove {
    
NavSystem::NavSystem(const InitParams& params)
{
    _bodies.reserve(params.numBodies);
}
    
NavBody* NavSystem::attachBody(NavBody* body)
{
    auto it = containerLowerBound(_bodies, body->entity());
    if (it == _bodies.end() || (*it)->entity() != body->entity()) {
        it = _bodies.emplace(it, body);
    }
    
    CK_ASSERT(body == *it);
    
    return *it;
}

NavBody* NavSystem::detachBody(Entity entity)
{
    auto it = containerLowerBound(_bodies, entity);
    if (it == _bodies.end() || (*it)->entity() != entity) {
        return nullptr;
    }
    
    NavBody* body = *it;
    _bodies.erase(it);
    return body;
}

NavBody* NavSystem::findBody(Entity entity)
{
    auto it = containerLowerBound(_bodies, entity);
    if (it == _bodies.end() || (*it)->entity() != entity) {
        return nullptr;
    }
    NavBody* body = *it;
    return body;
}


auto NavSystem::containerLowerBound
(
    NavBodyContainer& container,
    Entity entity
)
->
NavBodyContainer::iterator
{
    auto it = std::lower_bound(container.begin(), container.end(), entity,
                    [](const NavBody* body, Entity e) -> bool {
                        return body->entity() < e;
                    });
    return it;
}
    
    } /* namespace ove */
} /* namespace cinek */