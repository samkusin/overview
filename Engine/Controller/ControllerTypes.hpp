//
//  ControllerTypes.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 3/1/16.
//
//

#ifndef Overview_ControllerTypes_hpp
#define Overview_ControllerTypes_hpp

#include "Engine/EngineTypes.hpp"

#include <cinek/objectpool.hpp>

namespace cinek {
    namespace ove {

using TransformIndex = int;
    
using TransformSetPool = ManagedObjectPool<TransformSet, void>;
using TransformSetHandle = ManagedHandle<TransformSet, TransformSetPool>;
    
    } /* namespace ove */
} /* namespace cinek */


#endif /* ControllerTypes_hpp */
