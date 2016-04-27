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
#include <bullet/btBulletCollisionCommon.h>

#include <vector>
#include <array>

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
    struct InitParams
    {
        int staticLimit;
        std::array<int, SceneBody::kNumCategories> limits;
        
        InitParams() {
            staticLimit = 0;
            limits.fill(0);
        }
    };
        
    Scene(const InitParams& initParams, btIDebugDraw* debugDrawer=nullptr);
    ~Scene();

    /**
     *  Updates the scene simulations using the specified timestep.  It's best
     *  to run this using a fixed timestep, though this requirement depends
     *  on the needs of an application.
     *
     *  @param  dt      Time step in seconds
     */
    void simulate(CKTimeDelta dt);
    /**
     *  Deactivates dynamic physics simulation
     *
     *  Useful for freezing the simulation.  Collision detection is still
     *  active.
     */
    void deactivate();
    /**
     *  Activates dynamic physics simulation if inactive
     *
     *  Useful for continuing the dynamic simulation.
     */
    void activate();
    
    /**
     *  @return Simulation activation status
     */
    bool isActive() const;
    /**
     *   Adds a fixed body to the Scene.  The hull is managed by the Scene.
     */
    SceneBody* attachBody(SceneBody* body, uint32_t categoryMask);
    /**
     *  Removes the body from the Scene.
     */
    SceneBody* detachBody(Entity entity);
    /**
     *  @param  entity  What entity to find a body for
     */
    SceneBody* findBody(Entity entity, uint32_t categoryMask=SceneBody::kAllCategories) const;
    /**
     *  Add categories to a body
     */
    SceneBody* addCategoryToBody(Entity entity, uint32_t category);
    /**
     *  Remove categories from a body
     */
    SceneBody* removeCategoryFromBody(Entity entity, uint32_t category);
    /**
     *  Retrieve the closest hit point with a given ray.
     *  
     *  @param  origin  The ray origin
     *  @param  dir     The ray normalized direction
     *  @param  dist    The ray distance from origin
     */
    SceneRayTestResult rayTestClosest(const btVector3& origin,
        const btVector3& dir,
        btScalar dist,
        uint16_t includeFilters = SceneBody::kAllFilter,
        uint16_t excludeFilters = 0) const;
    
    /**
     *  Retrieve body objects using a filter and category mask.
     *  The filter function should have the following prototype:
     *      fn(SceneBody* body, uint32_t categoryFilterMask)
     */
    template<typename Fn>
    void iterateBodies(uint32_t catagoryMask, Fn filterFn) const;
        
    /**
     *  Executes per render frame updates.
     */
    void debugRender();
    
private:
    using SceneBodyContainer = std::vector<SceneBody*>;
    
    SceneBodyContainer _bodies;

    std::array<SceneBodyContainer, SceneBody::kNumCategories> _containers;
    
    SceneBodyContainer::iterator sceneContainerLowerBound
    (
        SceneBodyContainer& container,
        Entity entity
    );
    
    SceneBody* addCategoryToBody(SceneBody* body, uint32_t category);
    SceneBody* removeCategoryFromBody(SceneBody* body, uint32_t category);
    
    void addBodyToBtWorld(SceneBody* body);
    void removeBodyFromBtWorld(SceneBody* body);
    
    bool _simulateDynamics;
    
    btDefaultCollisionConfiguration _btCollisionConfig;
    btCollisionDispatcher _btCollisionDispatcher;
    btDbvtBroadphase _btBroadphase;
    btCollisionWorld _btWorld;
};

template<typename Fn>
void Scene::iterateBodies
(
    uint32_t categoryMask,
    Fn filterFn
)
const
{
    if (!categoryMask)
        return;
    
    SceneBodyContainer::const_iterator it = _bodies.begin();
    SceneBodyContainer::const_iterator itEnd = _bodies.end();
    
    if (categoryMask != SceneBody::kAllCategories) {
        //  check if we're searching just one category
        //  http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
        if ((categoryMask != 0) &&
            ((categoryMask & (~categoryMask + 1)) == categoryMask)) {
        
            uint32_t i = 0, categories = categoryMask;
            while (categories) {
                if (categories & 1) {
                    it = _containers[i].begin();
                    itEnd = _containers[i].end();
                    break;
                }
                ++i;
                categories >>= 1;
            }
         
            for (; it != itEnd; ++it) {
                auto body = *it;
                if ((body->categoryMask & categoryMask) != 0) {
                    filterFn(*it, categoryMask);
                }
            }
                
            return;
        }
    }

    for (; it != itEnd; ++it) {
        filterFn(*it, categoryMask);
    }
}

    
    } /* namespace ove */
} /* namespace cinek  */


#endif /* Overview_Scene_hpp */
