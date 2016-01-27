//
//  Scene.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/20/15.
//
//

#include "Scene.hpp"
#include "SceneDataContext.hpp"

#include <cinek/objectpool.inl>

#include <bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

namespace cinek {

    template class ObjectPool<ove::SceneBody>;
    
    namespace ove {
    
////////////////////////////////////////////////////////////////////////////////

void activate(SceneBody& body)
{
    auto& savedState = body.savedState;
    
    /*
    auto btBody = body.btBody;
    
    if ((savedState.flags & SceneBody::SavedState::kDynamic)!=0) {
        btBody->setMassProps(savedState.massProps.mass, savedState.massProps.inertia);
    }
    */
    savedState.flags = SceneBody::SavedState::kNone;
}

void deactivate(SceneBody& body)
{
    /*
    auto& savedState = body.savedState;
    auto btBody = body.btBody;
    
    if (!btBody->isStaticOrKinematicObject()) {
        savedState.flags |= SceneBody::SavedState::kDynamic;
        
        savedState.massProps.fromRigidBody(*btBody);
        btBody->setMassProps(btScalar(0), btVector3(0,0,0));\
    }
    */
}

Scene::Scene(btIDebugDraw* debugDrawer) :
    _simulateDynamics(true),
    _btCollisionDispatcher(&_btCollisionConfig),
    _btWorld(&_btCollisionDispatcher,
             &_btBroadphase,
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
    _btWorld.performDiscreteCollisionDetection();
    //_btWorld.stepSimulation(dt);
}

void Scene::deactivateSimulation()
{
    if (_simulateDynamics) {
        for (auto& obj : _objects) {
            deactivate(*obj);
        }
        
        _simulateDynamics = false;
    }
}

void Scene::activateSimulation()
{
    if (!_simulateDynamics) {
        for (auto& obj : _objects) {
            activate(*obj);
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

SceneBody* Scene::attachBody
(
    SceneBody* body
)
{
    auto it = std::lower_bound(_objects.begin(), _objects.end(), body->entity,
        [](const SceneBody* obj0, Entity e) -> bool {
            return obj0->entity < e;
        });
    if (it != _objects.end() && (*it)->entity == body->entity)
        return nullptr;
    
    body->savedState.flags = SceneBody::SavedState::kNone;
    
    //  reflect current simulation state
    if (!_simulateDynamics) {
        deactivate(*body);
    }

    body->btBody->setUserPointer(body);
    _objects.emplace(it, body);
    _btWorld.addCollisionObject(body->btBody);
    
    if (body->motionState) {
        btTransform btTransform;
        body->motionState->getWorldTransform(btTransform);
        body->btBody->setWorldTransform(btTransform);
    }
    
    return body;
}

SceneBody* Scene::detachBody
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
    
    auto body = *it;
    auto obj = body->btBody;
    
    _btWorld.removeCollisionObject(obj);
    _objects.erase(it);
    
    return body;
}

SceneBody* Scene::findBody(Entity entity) const
{
    auto it = std::lower_bound(_objects.begin(), _objects.end(), entity,
        [](const SceneBody* obj0, Entity e) -> bool {
            return obj0->entity < e;
        });
    if (it == _objects.end() || (*it)->entity != entity)
        return nullptr;
    
    auto obj = *it;
    return obj;
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
        result.body = body;
        result.normal = cb.m_hitNormalWorld;
        result.position = cb.m_hitPointWorld;
    }
    
    return result;
}



        
    }   /* namespace ove */
}   /* namespace cinek */