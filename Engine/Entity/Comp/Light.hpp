//
//  Light.hpp
//  Overview
//
//  Created by Samir Sinha on 5/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Component_Light_hpp
#define Overview_Entity_Component_Light_hpp

#include "Engine/Entity/EntityTypes.hpp"

namespace cinek { namespace overview {

namespace component
{
    struct Light
    {
        COMPONENT_DEFINITION(Light);
        
        enum Type { kPoint, kDirectional };
        
        Type type;
        Vector4 color;
        float strength;
        float falloffDistance;
        float falloffLimit;
    };
}

} /* namespace overview */ } /* namespace cinek */


#endif
