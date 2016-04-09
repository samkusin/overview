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

Scene::Scene
(
    const InitParams& initParams, 
    btIDebugDraw* debugDrawer
) :
    _simulateDynamics(true),
    _btCollisionDispatcher(&_btCollisionConfig),
    _btWorld(&_btCollisionDispatcher,
             &_btBroadphase,
             &_btCollisionConfig)
{
    int count = initParams.staticLimit;
    
    CK_ASSERT(initParams.limits.size() == _containers.size());
    
    for (int i = 0; i < initParams.limits.size(); ++i) {
        if (initParams.limits[i] > 0) {
            _containers[i].reserve(initParams.limits[i]);
            count += initParams.limits[i];
        }
    }
    _bodies.reserve(count);
    
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
            if (body->checkFlags(SceneBody::kIsDynamic)) {
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
    if (it != _bodies.end() && (*it)->entity == body->entity) {
        CK_ASSERT(false);
        return *it;
    }
    //  reflect current simulation state
    if (!_simulateDynamics) {
        ove::deactivate(*body);
    }

    body->btBody->setUserPointer(body);
    
    if (body->motionState) {
        btTransform btTransform;
        body->motionState->getWorldTransform(btTransform);
        body->btBody->setWorldTransform(btTransform);
    }
    
    body->categoryMask = 0;
    
    uint32_t category = 0;
    while (categories) {
        if ((categories & 1) != 0) {
            addCategoryToBody(body, category);
        }
        ++category;
        categories >>= 1;
    }
    
    it = _bodies.emplace(it, body);
    CK_ASSERT(body == *it);
    
    //  update btWorld
    addBodyToBtWorld(body);
    
    return body;
}

SceneBody* Scene::addCategoryToBody(Entity entity, uint32_t category)
{
    CK_ASSERT_RETURN_VALUE(category < SceneBody::kNumCategories, nullptr);

    SceneBody* body = findBody(entity);
    if (body) {
        removeBodyFromBtWorld(body);
        addCategoryToBody(body, category);
        addBodyToBtWorld(body);
    }
    return body;
}


SceneBody* Scene::addCategoryToBody(SceneBody* body, uint32_t category)
{
    auto& container = _containers[category];
    auto it = sceneContainerLowerBound(container, body->entity);
    if (it == container.end() || (*it)->entity != body->entity) {
        it = container.emplace(it, body);
        uint32_t categoryFlag = 1 << category;
        body->categoryMask |= categoryFlag;
    }
    return body;
}

SceneBody* Scene::removeCategoryFromBody(Entity entity, uint32_t category)
{
    SceneBody* body = findBody(entity);
    if (body) {
        removeBodyFromBtWorld(body);
        removeCategoryFromBody(body, category);
        addBodyToBtWorld(body);
    }
    return body;
}

SceneBody* Scene::removeCategoryFromBody(SceneBody* body, uint32_t category)
{
    auto& container = _containers[category];
    auto it = sceneContainerLowerBound(container, body->entity);
    if (it != container.end() && (*it)->entity == body->entity) {
        body->categoryMask &= ~(1 << category);
        container.erase(it);
    }
    return body;
}


SceneBody* Scene::detachBody
(
    Entity entity
)
{
    SceneBody* body = nullptr;

    auto it = std::lower_bound(_bodies.begin(), _bodies.end(), entity,
        [](const SceneBody* obj0, Entity e) -> bool {
            return obj0->entity < e;
        });
    
    CK_ASSERT_RETURN_VALUE(it != _bodies.end() && (*it)->entity == entity, nullptr);
    body = *it;
    
    //  remove from all categories
    uint32_t categoryMask = body->getCategoryMask();
    uint32_t category = 0;
    while (categoryMask) {
        if ((categoryMask & (1 << category)) != 0) {
            removeCategoryFromBody(body, category);
            categoryMask = body->categoryMask;
        }
        ++category;
    }
    
    removeBodyFromBtWorld(body);
    _bodies.erase(it);
    
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
    float dist,
    uint16_t includeFilters,
    uint16_t excludeFilters
)
const
{
    SceneRayTestResult result;
    
    btVector3 to { dir };
    to *= dist;
    to += origin;
    
    btCollisionWorld::ClosestRayResultCallback cb(origin, to);
    cb.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
    cb.m_collisionFilterMask = includeFilters ^ excludeFilters;
    
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

void Scene::addBodyToBtWorld(SceneBody* body)
{
    auto categoryMask = body->getCategoryMask();
    if ((categoryMask & SceneBody::kIsDynamic) != 0) {
        body->btBody->setCollisionFlags(0);
    }
    else {
        body->btBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
    }
    
    short btGroup = 0;
    short btMask = SceneBody::kAllFilter;
    
    if ((categoryMask & SceneBody::kIsStaging) != 0) {
        //  staged bodies collide with everything
        btGroup = SceneBody::kStagingFilter;
    }
    else if ((categoryMask & SceneBody::kIsDynamic) != 0) {
        //  bodies collide with everything
        /// note, this could be a bit more granular as needed
        btGroup = SceneBody::kDefaultFilter;
    }
    else {
        //  statics do not collide with each other
        btGroup = SceneBody::kStaticFilter;
        btMask = btMask ^ SceneBody::kStaticFilter;
    }
    
    _btWorld.addCollisionObject(body->btBody, btGroup, btMask);
}

void Scene::removeBodyFromBtWorld(SceneBody* body)
{
    auto btBody = body->btBody;
    
    _btWorld.removeCollisionObject(btBody);
}
/*
void Scene::updateBodyInBtWorld(SceneBody* body, bool addToWorld)
{
    auto categoryMask = body->getCategoryMask();
    
    if (body->btBody->getCollisionFlags())
    
    
    
 
        
 
}
 */
    }   /* namespace ove */
}   /* namespace cinek */