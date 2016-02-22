//
//  NavSystem.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/17/16.
//
//

#include "NavSystem.hpp"
#include "NavBody.hpp"
#include "NavPath.hpp"
#include "Pathfinder.hpp"

#include "Engine/Debug.hpp"

#include <algorithm>

namespace cinek {
    namespace ove {
    
NavSystem::NavSystem(const InitParams& params) :
    _pathfinder(params.pathfinder)
{
    _bodies.reserve(params.numBodies);
}

NavSystem::~NavSystem()
{
    _pathfinder->cancelByListener(this);
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
    return const_cast<NavBody*>(static_cast<const NavSystem*>(this)->findBody(entity));
}

const NavBody* NavSystem::findBody(Entity entity) const
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

auto NavSystem::containerLowerBound
(
    const NavBodyContainer& container,
    Entity entity
)
const -> NavBodyContainer::const_iterator
{
    auto it = std::lower_bound(container.cbegin(), container.cend(), entity,
                    [](const NavBody* body, Entity e) -> bool {
                        return body->entity() < e;
                    });
    return it;
}

void NavSystem::moveBodyToPosition(Entity entity, ckm::vector3f pos)
{
    const NavBody* body = findBody(entity);
    if (body) {
        _pathfinder->generatePath(this, entity, body->position(), pos);
    }
}

    
void NavSystem::startFrame()
{
    for (auto& body : _bodies) {
        body->updateTransform();
    }
}

void NavSystem::simulate(double dt)
{
    for (auto& body : _bodies) {
    
    }
}

void NavSystem::endFrame()
{
}

void NavSystem::onPathfinderPathUpdate
(
    Entity entity,
    NavPath&& path
)
{
    OVENGINE_LOG_INFO("NavSystem - Path for entity %" PRIu64 " generated of size %d\n", entity, path.size());
}

void NavSystem::onPathfinderError
(
    Entity entity,
    PathfinderError error
)
{
}
    
    } /* namespace ove */
} /* namespace cinek */