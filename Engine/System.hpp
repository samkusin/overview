//
//  System.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 3/10/16.
//
//

#ifndef Overview_System_hpp
#define Overview_System_hpp

#include "EngineTypes.hpp"

#include <vector>

namespace cinek {
    namespace ove {

template<typename Body, typename Derived>
class System
{
public:
    System(uint32_t numBodies);

    Body* attachBody(Body* body);
    Body* detachBody(Entity entity);
    
    Body* findBody(Entity entity);
    const Body* findBody(Entity entity) const;
    
    //  framework
    void activate();
    void startFrame();
    void simulate(CKTimeDelta dt);
    void endFrame();
    void deactivate();
    
protected:
    using BodyContainer = std::vector<Body*>;
    BodyContainer _bodies;

    typename BodyContainer::iterator containerLowerBound(
        BodyContainer& container,
        Entity entity
    );
    typename BodyContainer::const_iterator containerLowerBound(
        const BodyContainer& container,
        Entity entity
    ) const;

};
    
    } /* namespace ove */
} /* namespace cinek */


#endif /* Overview_System_hpp */
