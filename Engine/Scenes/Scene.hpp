/**
 * @file        Scene.hpp
 * @author      Samir Sinha
 * @date        11/20/2015
 * @brief       Definitions for the Scene class
 * @copyright   Copyright 2015 Cinekine.  
 *
 */

#ifndef Overview_Scene_hpp
#define Overview_Scene_hpp

#include "Engine/EngineTypes.hpp"

#include <cinek/objectpool.hpp>
#include <cinek/allocator.hpp>
#include <bullet/btBulletDynamicsCommon.h>

#include <vector>

namespace cinek {
    namespace ove {
     
/**
 *  @class  Scene
 *  @brief  A Scene represents the physical world of a simulation.
 *
 *  The Scene applies the physical components for an Entity.  Its interface
 *  abstracts the details (such as a Rigid Body Physics engine.)  Controllers
 *  place entities within a Scene for simulation.
 *
 */
class Scene
{
public:
    Scene(btIDebugDraw* debugDrawer=nullptr);
    ~Scene();

    /**
     *   Adds a fixed body to the Scene.  The hull is managed by the Scene.
     */
    btRigidBody* attachBody(btRigidBody* object, Entity entity);
    /**
     *  Removes the body from the Scene.
     */
    btRigidBody* detachBody(Entity entity);
    /**
     *  @param  What entity to find a body for
     */
    btRigidBody* findBody(Entity entity) const;
    /**
     *  Updates the scene simulations using the specified timestep.  It's best
     *  to run this using a fixed timestep, though this requirement depends
     *  on the needs of an application.
     *
     *  @param  dt      Time step in seconds
     */
    void simulate(double dt);
    /**
     *  Executes per render frame updates.
     */
    void debugRender();
    
private:
    struct SceneBody
    {
        btRigidBody* btBody;
        Entity entity;
    };
    ObjectPool<SceneBody> _bodies;
    std::vector<SceneBody*> _objects;
    
    btDefaultCollisionConfiguration _btCollisionConfig;
    btCollisionDispatcher _btCollisionDispatcher;
    btDbvtBroadphase _btBroadphase;
    btSequentialImpulseConstraintSolver _btConstraintSolver;
    btDiscreteDynamicsWorld _btWorld;
};
    
    } /* namespace ove */
} /* namespace cinek  */

#endif /* Overview_Scene_hpp */
