//
//  Scene.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/20/15.
//
//

#include "Scene.hpp"
#include "SceneObject.hpp"

namespace cinek {
    namespace ove {
    
////////////////////////////////////////////////////////////////////////////////

Scene::Scene(btIDebugDraw* debugDrawer) :
    _bodies(256),
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
    
    _btWorld.removeRigidBody(obj->btBody);

    _objects.erase(it);
    return obj->btBody;
}

        
    }   /* namespace ove */
}   /* namespace cinek */