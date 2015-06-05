//
//  Components.cpp
//  Overview
//
//  Created by Samir Sinha on 5/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "EntityTypes.hpp"
#include "Light.hpp"
#include "Camera.hpp"
#include "Renderable.hpp"
#include "Transform.hpp"


namespace cinek { namespace overview {

namespace component
{
    COMPONENT_IMPLEMENTATION(Light);
    COMPONENT_IMPLEMENTATION(Camera);
    COMPONENT_IMPLEMENTATION(Transform);
    COMPONENT_IMPLEMENTATION(Renderable);
}

} /* namespace overview */ } /* namespace cinek */
