//
//  TransformSystem.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 3/10/16.
//
//

#ifndef Overview_Controller_TransformSystem_hpp
#define Overview_Controller_TransformSystem_hpp

#include "Engine/System.hpp"

#include "ControllerTypes.hpp"

namespace cinek {
    namespace ove {
    
class TransformSystem : public System<TransformBody, TransformSystem>
{
public:
    struct InitParams
    {
        uint32_t numBodies = 16;
    };
    TransformSystem(const InitParams& params);
    ~TransformSystem();
    
    //  framework
    void activate();
    void startFrame();
    void simulate(CKTimeDelta dt);
    void endFrame();
    void deactivate();
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_Controller_TransformSystem_hpp */
