//
//  NavSystem.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/17/16.
//
//

#ifndef Overview_Controller_NavSystem_hpp
#define Overview_Controller_NavSystem_hpp

#include "ControllerTypes.hpp"

#include "Engine/Path/PathfinderListener.hpp"

#include <vector>

namespace cinek {
    namespace ove {
    
//  The NavSystem move bodies along paths generated by the Pathfinder.
//
class NavSystem : public PathfinderListener
{
public:
    struct InitParams
    {
        Pathfinder* pathfinder;
        uint32_t numBodies = 16;
    };
    NavSystem(const InitParams& params);
    ~NavSystem();
    
    NavBody* attachBody(NavBody* body);
    NavBody* detachBody(Entity entity);
    
    NavBody* findBody(Entity entity);
    const NavBody* findBody(Entity entity) const;
    
    //  generates and executes a path to the target
    void moveBodyToPosition(Entity body, ckm::vector3f pos);
    
    //  framework
    void activate();
    void startFrame();
    void simulate(CKTimeDelta dt);
    void endFrame();
    void deactivate();
    
protected:
    virtual void onPathfinderPathUpdate(Entity entity, NavPath&& path) override;
    virtual void onPathfinderError(Entity entity, PathfinderError error) override;
    
private:
    ckm::vector3f steer(const NavPath& path, const ckm::vector3f& position,
        ckm::scalar dist) const;
    
    ckm::vector3f turn(const ckm::matrix3f& orient, const ckm::vector3f& direction,
        ckm::scalar rotation) const;
    
private:
    using NavBodyContainer = std::vector<NavBody*>;
    NavBodyContainer _bodies;
    Pathfinder* _pathfinder;
    NavPathQueryPtr _query;
    bool _active;

    NavBodyContainer::iterator containerLowerBound(
        NavBodyContainer& container,
        Entity entity
    );
    NavBodyContainer::const_iterator containerLowerBound(
        const NavBodyContainer& container,
        Entity entity
    ) const;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_Controller_NavSystem_hpp */