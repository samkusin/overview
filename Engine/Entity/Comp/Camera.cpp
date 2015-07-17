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
    _type = kPerspective;
    _dirty = true;
}

void Camera::initOrtho(int viewIndex, ckm::scalar nearZ, ckm::scalar farZ)
{
    _viewIndex = viewIndex;
    _nearZClip = nearZ;
    _farZClip = farZ;
    _fov = 0;
    _type = kOrthogonal;
    _dirty = true;
}

void Camera::initNull(int viewIndex)
{
    _viewIndex = viewIndex;
    _nearZClip = 0;
    _farZClip = 0;
    _fov = 0;
    _type = kNull;
    _dirty = true;
}

}

} /* namespace overview */ } /* namespace cinek */
