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

#include <vector>

namespace cinek {
    namespace ove {
    
class NavSystem
{
public:
    struct InitParams
    {
        uint32_t numBodies = 16;
    };
    NavSystem(const InitParams& params);
    
    NavBody* attachBody(NavBody* body);
    NavBody* detachBody(Entity entity);
    
    NavBody* findBody(Entity entity);
    
private:
    using NavBodyContainer = std::vector<NavBody*>;
    NavBodyContainer _bodies;
    
    NavBodyContainer::iterator containerLowerBound
    (
        NavBodyContainer& container,
        Entity entity
    );
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_NavSystem_hpp */
