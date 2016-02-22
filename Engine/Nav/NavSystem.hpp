//
//  NavSystem.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/17/16.
//
//

#ifndef Overview_NavSystem_hpp
#define Overview_NavSystem_hpp

#include "PathTypes.hpp"
#include "PathfinderListener.hpp"

#include <vector>

namespace cinek {
    namespace ove {
    
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
    void startFrame();
    void simulate(double dt);
    void endFrame();
    
protected:
    virtual void onPathfinderPathUpdate(Entity entity, NavPath&& path) override;
    virtual void onPathfinderError(Entity entity, PathfinderError error) override;
    
private:
    using NavBodyContainer = std::vector<NavBody*>;
    NavBodyContainer _bodies;
    Pathfinder* _pathfinder;
    
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

#endif /* Overview_NavSystem_hpp */
