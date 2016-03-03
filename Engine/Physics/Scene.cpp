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
}

void deactivate(SceneBody& body)
{
}

auto Scene::sceneContainerLowerBound
(
    SceneBodyContainer& container,
    Entity entity
)
-> SceneBodyContainer::iterator
{
    auto it = std::lower_bound(container.begin(), container.end(), entity,
        [](const SceneBody* obj0, Entity e) -> bool {
            return obj0->entity < e;
        });
    return it;
}

Scene::Scene(btIDebugDraw* debugDrawer) :
    _simulateDynamics(true),
    _btCollisionDispatcher(&_btCollisionConfig),
    _btWorld(&_btCollisionDispatcher,
             &_btBroadphase,
             &_btCollisionConfig)
{
    _bodies.reserve(256);
    
    _btWorld.setDebugDrawer(debugDrawer);
}

Scene::~Scene()
{
}
    
void Scene::simulate(CKTimeDelta dt)
{
    _btWorld.performDiscreteCollisionDetection();

    if (_simulateDynamics) {
    
        for (auto& body : _bodies) {
            if (body->isInCategory(SceneBody::kObject)) {
                if (body->transformChanged || body->velocityChanged) {
                    btTransform& transform = body->btBody->getWorldTransform();
                    
                    // set btBody translate
                    btVector3 nextPos = transform.getOrigin() + body->linearVelocity;
                    transform.setOrigin(nextPos);
                    
                    // set btBody basis (TODO: optimize)
                    if (!body->angularVelocity.fuzzyZero()) {
                        btVector3 rotAxis = body->angularVelocity.normalized();
                        btScalar rotAngle = body->angularVelocity.length();
                        
                        btQuaternion rotQ(rotAxis, rotAngle);
                        //rotQ.normalize();
                        
                        btMatrix3x3 rotMtx(rotQ);
                        transform.setBasis(rotMtx * transform.getBasis());
                    }
                    
                    //  updates revision
                    body->btBody->setWorldTransform(transform);
                    
                    // update motion state?
                    body->motionState->setWorldTransform(transform);
                }
            }
        }
    
    }
}

void Scene::deactivate()
{
    if (_simulateDynamics) {
        for (auto& obj : _bodies) {
            ove::deactivate(*obj);
        }
        
        _simulateDynamics = false;
    }
}

void Scene::activate()
{
    if (!_simulateDynamics) {
        for (auto& obj : _bodies) {
            ove::activate(*obj);
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
    SceneBody* body,
    uint32_t categories
)
{
    auto it = sceneContainerLowerBound(_bodies, body->entity);
    
    //  is it a new body? then add it to our master list
    if (it == _bodies.end() || (*it)->entity != body->entity) {
        //  reflect current simulation state
        if (!_simulateDynamics) {
            ove::deactivate(*body);
        }

        body->btBody->setUserPointer(body);
        it = _bodies.emplace(it, body);
        _btWorld.addCollisionObject(body->btBody);
        
        if (body->motionState) {
            btTransform btTransform;
            body->motionState->getWorldTransform(btTransform);
            body->btBody->setWorldTransform(btTransform);
        }
        
        body->categoryMask = 0;
    }
    
    CK_ASSERT(body == *it);
    
    uint32_t index = 0;
    while (categories) {
        if ((categories & 1) != 0) {
            auto& container = _containers[index];
            it = sceneContainerLowerBound(container, body->entity);
            if (it == container.end() || (*it)->entity != body->entity) {
                it = container.emplace(it, body);
                body->categoryMask |= (1 << index);
            }
        }
        ++index;
        categories >>= 1;
    }
    
    return body;
}

SceneBody* Scene::detachBody
(
    Entity entity,
    uint32_t categories
)
{
    SceneBody* body = nullptr;
    uint32_t index = 0;

    //  remove from all categories
    uint32_t categoryMask = categories;
    while (categoryMask && (body && body->categoryMask)) {
        if ((categoryMask & 1) != 0) {
            auto& container = _containers[index];
            auto it = sceneContainerLowerBound(container, entity);
            if (it != container.end() && (*it)->entity == entity) {
                body = *it;
                body->categoryMask &= ~(1 << index);
                _bodies.erase(it);
            }
        }
        ++index;
        categoryMask >>= 1;
    }
    
    //  remove from the global list
    if ((body && !body->categoryMask) || categories == SceneBody::kAllCategories) {
        auto it = std::lower_bound(_bodies.begin(), _bodies.end(), entity,
            [](const SceneBody* obj0, Entity e) -> bool {
                return obj0->entity < e;
            });
        
        CK_ASSERT_RETURN_VALUE(it != _bodies.end() && (*it)->entity == entity, nullptr);
        CK_ASSERT(!body || body == *it);
        
        body = *it;
    
        auto obj = body->btBody;
    
        _btWorld.removeCollisionObject(obj);
        _bodies.erase(it);
    }
    
    return body;
}

SceneBody* Scene::findBody
(
    Entity entity,
    uint32_t categoryMask
)
const
{
    if (!categoryMask)
        return nullptr;
    
    SceneBodyContainer::const_iterator it = _bodies.begin();
    SceneBodyContainer::const_iterator itEnd = _bodies.end();
    
    if (categoryMask != SceneBody::kAllCategories) {
        //  check if we're searching just one category
        //  http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
        if ((categoryMask != 0) &&
            ((categoryMask & (~categoryMask + 1)) == categoryMask)) {
        
            uint32_t i = 0;
            while (categoryMask) {
                if (categoryMask & 0x01) {
                    it = _containers[i].begin();
                    itEnd = _containers[i].end();
                    break;
                }
                ++i;
                categoryMask >>= 1;
            }
        }
    }
    
    auto findIt = std::lower_bound(it, itEnd, entity,
        [](const SceneBody* obj0, Entity e) -> bool {
            return obj0->entity < e;
        });
    if (findIt == _bodies.end() || (*findIt)->entity != entity)
        return nullptr;
    
    auto obj = *findIt;
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
    else {
        result.body = nullptr;
    }
    
    return result;
}



        
    }   /* namespace ove */
}   /* namespace cinek */