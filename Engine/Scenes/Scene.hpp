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

#include "SceneTypes.hpp"

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
    SceneBody* attachBody(btRigidBody* object, Entity entity);
    /**
     *  Removes the body from the Scene.
     */
    btRigidBody* detachBody(Entity entity);
    /**
     *  @param  entity  What entity to find a body for
     */
    SceneBody* findBody(Entity entity) const;
    /**
     *  Updates the scene simulations using the specified timestep.  It's best
     *  to run this using a fixed timestep, though this requirement depends
     *  on the needs of an application.
     *
     *  @param  dt      Time step in seconds
     */
    void simulate(double dt);
    /**
     *  Deactivates dynamic physics simulation
     *
     *  Useful for freezing the simulation.  Collision detection is still
     *  active.
     */
    void deactivateSimulation();
    /**
     *  Activates dynamic physics simulation if inactive
     *
     *  Useful for continuing the dynamic simulation.
     */
    void activateSimulation();
    /**
     *  @return Simulation activation status
     */
    bool isActive() const;
    /**
     *  Executes per render frame updates.
     */
    void debugRender();
    /**
     *  Retrieve the closest hit point with a given ray.
     *  
     *  @param  origin  The ray origin
     *  @param  dir     The ray normalized direction
     *  @param  dist    The ray distance from origin
     */
    SceneRayTestResult rayTestClosest(const btVector3& origin,
        const btVector3& dir,
        btScalar dist) const;
    
private:
    ObjectPool<SceneBody> _bodies;
    std::vector<SceneBody*> _objects;
    
    bool _simulateDynamics;
    
    btDefaultCollisionConfiguration _btCollisionConfig;
    btCollisionDispatcher _btCollisionDispatcher;
    btDbvtBroadphase _btBroadphase;
    btSequentialImpulseConstraintSolver _btConstraintSolver;
    btDiscreteDynamicsWorld _btWorld;
};
    
    } /* namespace ove */
} /* namespace cinek  */

#endif /* Overview_Scene_hpp */
