//
//  Scene.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/20/15.
//
//

#include "Scene.hpp"

#include <cinek/objectpool.inl>

#include <bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

namespace cinek {

    template class ObjectPool<ove::SceneBody>;
    
    namespace ove {
    
////////////////////////////////////////////////////////////////////////////////

Scene::Scene(btIDebugDraw* debugDrawer) :
    _bodies(256),
    _simulateDynamics(true),
    _btCollisionDispatcher(&_btCollisionConfig),
    _btWorld(&_btCollisionDispatcher,
             &_btBroadphase,
             &_btConstraintSolver,
             &_btCollisionConfig)
{
    _objects.reserve(256);
    
    _btWorld.setDebugDrawer(debugDrawer);
}

Scene::~Scene()
{
}
    
void Scene::simulate(double dt)
{
    _btWorld.stepSimulation(dt);
}

void Scene::deactivateSimulation()
{
    if (_simulateDynamics) {
        for (auto& obj : _objects) {
            obj->deactivate();
        }
        
        _simulateDynamics = false;
    }
}

void Scene::activateSimulation()
{
    if (!_simulateDynamics) {
        for (auto& obj : _objects) {
            obj->activate();
        }
    
        _simulateDynamics = true;
    }
}

bool Scene::isActive() const
{
    return _simulateDynamics;
}

void Scene::debugRender()
{
    _btWorld.debugDrawWorld();
}

btRigidBody* Scene::attachBody
(
    btRigidBody* object,
    Entity entity
)
{
    auto it = std::lower_bound(_objects.begin(), _objects.end(), entity,
        [](const SceneBody* obj0, Entity e) -> bool {
            return obj0->entity < e;
        });
    if (it != _objects.end() && (*it)->entity == entity)
        return nullptr;
    
    auto body = _bodies.construct();
    if (!body)
        return nullptr;
    
    body->btBody = object;
    body->entity = entity;
    body->savedState.flags = SceneBody::SavedState::kNone;
    
    //  reflect current simulation state
    if (!_simulateDynamics) {
        body->deactivate();
    }

    object->setUserPointer(body);
    _objects.emplace(it, body);
    _btWorld.addRigidBody(object);
    
    return body->btBody;
}

btRigidBody* Scene::detachBody
(
    Entity entity
)
{
    auto it = std::lower_bound(_objects.begin(), _objects.end(), entity,
        [](const SceneBody* obj0, Entity e) -> bool {
            return obj0->entity < e;
        });
    if (it == _objects.end() || (*it)->entity != entity)
        return nullptr;
    
    auto obj = *it;
    auto body = obj->btBody;
    
    _btWorld.removeRigidBody(body);
    _objects.erase(it);
    
    _bodies.destruct(obj);
    
    return body;
}

btRigidBody* Scene::findBody(Entity entity) const
{
    auto it = std::lower_bound(_objects.begin(), _objects.end(), entity,
        [](const SceneBody* obj0, Entity e) -> bool {
            return obj0->entity < e;
        });
    if (it == _objects.end() || (*it)->entity != entity)
        return nullptr;
    
    auto obj = *it;
    return obj->btBody;
}

SceneRayTestResult Scene::rayTestClosest
(
    const btVector3& origin,
    const btVector3& dir,
    float dist
)
const
{
    SceneRayTestResult result;
    
    btVector3 to { dir };
    to *= dist;
    to += origin;
    
    btCollisionWorld::ClosestRayResultCallback cb(origin, to);
    cb.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
    
    _btWorld.rayTest(origin, to, cb);
    
    if (cb.hasHit()) {
        SceneBody* body = reinterpret_cast<SceneBody*>(cb.m_collisionObject->getUserPointer());
        result.body = body->btBody;
        result.entity = body->entity;
        result.normal = cb.m_hitNormalWorld;
        result.position = cb.m_hitPointWorld;
    }
    
    return result;
}



        
    }   /* namespace ove */
}   /* namespace cinek */