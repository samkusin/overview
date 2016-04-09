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
    
    if (_viewMode == Mode::kOrthogonal) {
        bx::mtxOrtho(projMtx,
            -4.0f,
            4.0f,
            -4.0f,
            4.0f,
            _viewFrustrum.nearZ(), _viewFrustrum.farZ());
    }
    else {
        bx::mtxProj(projMtx,
            180.0f * _viewFrustrum.fovRadians()/bx::pi,
            _viewFrustrum.aspect(),
            _viewFrustrum.nearZ(),
            _viewFrustrum.farZ(),
            false);
    }
    
    bx::mtxMul(viewProjMtx, viewMtx, projMtx);
    bx::mtxInverse(invProjMtx, projMtx);
}


Vector3 Camera::worldPosition() const
{
    return { worldMtx[12], worldMtx[13], worldMtx[14] };
}

Vector4 Camera::translateFromViewportCoordinate
(
    float z,
    const int32_t x,
    const int32_t y
)
const
{
    gfx::Vector4 pt = {
        (2.0f * x) / viewportRect.w - 1.0f,
        1.0f - (2.0f*y)/viewportRect.h,
        z,
        1.0f
    };
    
    pt.x *= z;
    pt.y *= z;
    
    return pt;
}

Vector3 Camera::rayFromViewportCoordinate
(
    const int32_t x,
    const int32_t y
)
const
{
    gfx::Vector3 dir;
    gfx::Vector4 pt = translateFromViewportCoordinate(1.0f, x, y);
    gfx::Vector4 eyePt;
    
    bx::vec4MulMtx(eyePt, pt, invProjMtx);
    
    bx::vec4MulMtx(pt, eyePt, worldMtx);
    bx::vec3Norm(dir, pt);

    return dir;
}

Vector2 Camera::worldToScreenCoordinates(const Vector4& worldPos, bool* onscreen) const
{
    Vector2 screen;
    
    const float kHalfWidth = 0.5f * viewportRect.w;
    const float kHalfHeight = 0.5f * viewportRect.h;
    
    Vector4 proj;
    bx::vec4MulMtx(proj, worldPos, viewProjMtx);
    float w = proj[3];
    
    if (onscreen) *onscreen = true;
    if (proj[0] < -w) { proj[0] = -w; if (onscreen) *onscreen = false; }
    if (proj[0] > w) { proj[0] = w; if (onscreen) *onscreen = false; }
    if (proj[1] < -w) { proj[1] = -w; if (onscreen) *onscreen = false; }
    if (proj[1] > w) { proj[1] = w; if (onscreen) *onscreen = false; }
    if (proj[2] < ckm::kEpsilon) { proj[2] = 0; if (onscreen) *onscreen = false; }
    if (proj[2] > w) { proj[2] = w; if (onscreen) *onscreen = false; }
    
    proj[0] /= w;
    proj[1] /= w;
    proj[2] /= w;
    
    screen.x = (proj[0] * kHalfWidth) + kHalfWidth;
    screen.y = -(proj[1] * kHalfHeight) + kHalfHeight;
    
    return screen;
}

    }   /* namespace gfx */
}   /* namespace cinek */