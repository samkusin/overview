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

Scene::Scene() :
    _btCollisionDispatcher(&_btCollisionConfig),
    _btWorld(&_btCollisionDispatcher,
             &_btBroadphase,
             &_btConstraintSolver,
             &_btCollisionConfig)
{
    _objects.reserve(256);
}

Scene::~Scene()
{
}
    
void Scene::simulate(double dt)
{
    _btWorld.stepSimulation(dt);
}

SceneBody* Scene::attachBody
(
    SceneBody* object
)
{
    auto it = std::lower_bound(_objects.begin(), _objects.end(), object,
        [](const SceneBody* obj0, const SceneBody* obj1) -> bool {
            return obj0->entity() < obj1->entity();
        });
    if (it != _objects.end() && (*it)->entity() == object->entity())
        return nullptr;
    
    auto body = *_objects.emplace(it, object);
    body->incRef();
    
    _btWorld.addRigidBody(body->impl());
    
    return body;
}

SceneBody* Scene::detachBody
(
    Entity entity
)
{
    auto it = std::lower_bound(_objects.begin(), _objects.end(), entity,
        [](const SceneBody* obj0, Entity e) -> bool {
            return obj0->entity() < e;
        });
    if (it == _objects.end() || (*it)->entity() != entity)
        return nullptr;
    
    auto obj = *it;
    
    _btWorld.removeRigidBody(obj->impl());
    
    obj->decRef();
    _objects.erase(it);
    return obj;
}

        
    }   /* namespace ove */
}   /* namespace cinek */