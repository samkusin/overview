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
#include "EntityHierarchy.hpp"
#include "Body.hpp"

namespace cinek { namespace overview {

namespace component
{
    COMPONENT_TYPE_IMPL(Light, kLight, {}, {});
    COMPONENT_TYPE_IMPL(Camera, kCamera, {}, {});
    COMPONENT_TYPE_IMPL(Renderable, kRenderable, {}, {});
    COMPONENT_TYPE_IMPL(EntityHierarchy, kEntityHierarchy, {}, {});
    COMPONENT_TYPE_IMPL(Body, kBody, {}, {});
}

} /* namespace overview */ } /* namespace cinek */
