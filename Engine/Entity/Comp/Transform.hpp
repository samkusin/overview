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
        COMPONENT_DEFINITION(Transform);
        
        enum
        {
            kScale      = 0x0001,
            kRotate     = 0x0002,
            kTranslate  = 0x0004
        };
        
        uint32_t flags;
        Vector3 scale;
        Vector3 rotate;
        Vector3 translate;
    };
}

} /* namespace overview */ } /* namespace cinek */


#endif
