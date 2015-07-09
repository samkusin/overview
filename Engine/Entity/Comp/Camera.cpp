//
//  Camera.cpp
//  Overview
//
//  Created by Samir Sinha on 7/7/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "Camera.hpp"

namespace cinek { namespace overview {

namespace component
{

COMPONENT_TYPE_IMPL(Camera, kCamera, {}, {});

void Camera::init
(
    int viewIndex,
    ckm::scalar fovRads,
    ckm::scalar nearZ,
    ckm::scalar farZ
)
{
    _viewIndex = viewIndex;
    _nearZClip = nearZ;
    _farZClip = farZ;
    _fov = fovRads;
    _dirty = true;
}

}

} /* namespace overview */ } /* namespace cinek */
