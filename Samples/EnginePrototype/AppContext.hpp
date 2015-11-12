//
//  AppContext.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/11/15.
//
//

#ifndef Sample_AppContext_hpp
#define Sample_AppContext_hpp

#include "Engine/EngineTypes.hpp"

namespace cinek {
    namespace ove {
    
struct AppContext
{
    ViewStack* viewStack;
    EntityUtility* entityUtility;
};

    }   /* namespace ove */
}   /* namespace cinek */

#endif /* AppContext_hpp */
