//
//  TransformBody.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 3/1/16.
//
//

#include "TransformBody.hpp"

namespace cinek {
    namespace ove {
    
TransformBody::TransformBody(Entity entity, TransformSetHandle handle) :
    _entity(entity),
    _transformAnim(nullptr),
    _transformSet(handle)
{
}
    
    
    } /* namespace ove */
} /* namespace cinek */