//
//  Transform.hpp
//  Overview
//
//  Created by Samir Sinha on 5/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Component_Transform_hpp
#define Overview_Entity_Component_Transform_hpp

#include "Engine/Entity/EntityTypes.hpp"

namespace cinek { namespace overview {

namespace component
{
    struct Transform
    {
        COMPONENT_DEFINITION(Transform, kTransform);
        
        Matrix4 matrix;
    };
}

} /* namespace overview */ } /* namespace cinek */


#endif
