//
//  System.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 3/10/16.
//
//

#include "System.hpp"

namespace cinek {
    namespace ove {
    
template<typename Body, typename Derived>
System<Body, Derived>::System(uint32_t numBodies)
{
    _bodies.reserve(numBodies);
}

template<typename Body, typename Derived>
Body* System<Body, Derived>::attachBody(Body* body)
{
    auto it = containerLowerBound(_bodies, body->entity());
    if (it == _bodies.end() || (*it)->entity() != body->entity()) {
        it = _bodies.emplace(it, body);
    }
    
    CK_ASSERT(body == *it);
    
    return *it;
}

template<typename Body, typename Derived>
Body* System<Body, Derived>::detachBody(Entity entity)
{
    auto it = containerLowerBound(_bodies, entity);
    if (it == _bodies.end() || (*it)->entity() != entity) {
        return nullptr;
    }
    
    Body* body = *it;
    _bodies.erase(it);
    return body;
}

template<typename Body, typename Derived>
Body* System<Body, Derived>::findBody(Entity entity)
{
    return const_cast<Body*>(static_cast<const System*>(this)->findBody(entity));
}

template<typename Body, typename Derived>
const Body* System<Body, Derived>::findBody(Entity entity) const
{
    auto it = containerLowerBound(_bodies, entity);
    if (it == _bodies.end() || (*it)->entity() != entity) {
        return nullptr;
    }
    Body* body = *it;
    return body;
}

template<typename Body, typename Derived>
auto System<Body, Derived>::containerLowerBound
(
    BodyContainer& container,
    Entity entity
)
->
typename BodyContainer::iterator
{
    auto it = std::lower_bound(container.begin(), container.end(), entity,
                    [](const Body* body, Entity e) -> bool {
                        return body->entity() < e;
                    });
    return it;
}

template<typename Body, typename Derived>
auto System<Body, Derived>::containerLowerBound
(
    const BodyContainer& container,
    Entity entity
)
const -> typename BodyContainer::const_iterator
{
    auto it = std::lower_bound(container.cbegin(), container.cend(), entity,
                    [](const Body* body, Entity e) -> bool {
                        return body->entity() < e;
                    });
    return it;
}

template<typename Body, typename Derived>
void System<Body, Derived>::activate()
{
    static_cast<Derived*>(this)->activate();
}

template<typename Body, typename Derived>
void System<Body, Derived>::deactivate()
{
    static_cast<Derived*>(this)->deactivate();
}

template<typename Body, typename Derived>
void System<Body, Derived>::startFrame()
{
    static_cast<Derived*>(this)->startFrame();
}

template<typename Body, typename Derived>
void System<Body, Derived>::simulate(CKTimeDelta dt)
{
    static_cast<Derived*>(this)->simulate(dt);
}

template<typename Body, typename Derived>
void System<Body, Derived>::endFrame()
{
    static_cast<Derived*>(this)->endFrame();
}
    
    } /* namespace ove */
} /* namespace cinek */
