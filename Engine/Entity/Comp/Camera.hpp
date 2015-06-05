//
//  Camera.hpp
//  Overview
//
//  Created by Samir Sinha on 5/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Component_Camera_hpp
#define Overview_Entity_Component_Camera_hpp

#include "Engine/Entity/EntityTypes.hpp"

namespace cinek { namespace overview {

namespace component
{
    struct Camera
    {
        COMPONENT_DEFINITION(Camera, kCamera);
        
        float fov;          // in degrees
        float nearZClip;
        float farZClip;
    };
}

} /* namespace overview */ } /* namespace cinek */

#endif
