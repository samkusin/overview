//
//  State.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/9/15.
//
//

#ifndef Overview_Controller_State_hpp
#define Overview_Controller_State_hpp

#include <string>

namespace cinek {
    namespace ove {
        
template <typename Controller>
class State
{
public:
    virtual void onBegin(Controller& controller) = 0;
    virtual void onUpdate(Controller& controller, CKTimeDelta dt) = 0;
    virtual void onFrameUpdate(Controller& controller, CKTimeDelta dt) = 0;
    virtual void onEnd(Controller& controller) = 0;
};

    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_Controller_State_hpp */
