//
//  Camera.cpp
//  SampleCommon
//
//  Created by Samir Sinha on 1/5/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

#include "Camera.hpp"

#include <bx/fpumath.h>

namespace cinek {
    namespace gfx {
    
void Camera::update()
{
    bx::mtxInverse(viewMtx, worldMtx);
    
    _viewFrustrum = cinek::gfx::Frustrum(near, far, M_PI * fovDegrees/180.0f,
        ((float)viewportRect.w)/viewportRect.h);
    
    bx::mtxProj(projMtx,
        180.0f * _viewFrustrum.fovRadians()/bx::pi,
        _viewFrustrum.aspect(),
        _viewFrustrum.nearZ(),
        _viewFrustrum.farZ(),
        false);
    
    bx::mtxInverse(invProjMtx, projMtx);
}


Vector3 Camera::worldPosition() const
{
    return { worldMtx[12], worldMtx[13], worldMtx[14] };
}

Vector3 Camera::rayFromViewportCoordinate
(
    const int32_t x,
    const int32_t y
)
const
{
    gfx::Vector3 dir;
    
    gfx::Vector4 pt = {
        (2.0f * x) / viewportRect.w - 1.0f,
        1.0f - (2.0f*y)/viewportRect.h,
        1.0f,
        1.0f
    };
    
    gfx::Vector4 eyePt;
    bx::vec4MulMtx(eyePt, pt, invProjMtx);
    
    bx::vec4MulMtx(pt, eyePt, worldMtx);
    bx::vec3Norm(dir, pt);

    return dir;
}

    }   /* namespace gfx */
}   /* namespace cinek */