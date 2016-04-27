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

Vector4 Camera::viewTranslateFromScreenCoordinate
(
    const int32_t vx,
    const int32_t vy
)
const
{
    Vector4 projectedTranslate = {
        (2.0f * vx) / viewportRect.w - 1.0f,
        1.0f - (2.0f * vy)/viewportRect.h,
        1.0f,
        1.0f
    };
    Vector4 viewTranslate;
    bx::vec4MulMtx(viewTranslate, projectedTranslate, invProjMtx);
    return viewTranslate;
}

Vector3 Camera::viewRayFromScreenCoordinate
(
    const int32_t vx,
    const int32_t vy
)
const
{
    Vector3 ray;
    Vector4 translate = viewTranslateFromScreenCoordinate(vx, vy);
    bx::vec3Norm(ray, translate);
    return ray;
}

Vector3 Camera::worldRayFromScreenCoordinate
(
    const int32_t vx,
    const int32_t vy
)
const
{
    Vector3 worldRay;
    Vector4 viewTranslate = viewTranslateFromScreenCoordinate(vx, vy);
    Vector4 worldTranslate;
    bx::vec4MulMtx(worldTranslate, viewTranslate, worldMtx);
    bx::vec3Norm(worldRay, worldTranslate);
    
    return worldRay;
}

Vector3 Camera::viewPositionFromScreenCoordinate
(
    const int32_t vx, const int32_t vy,
    const float z
)
const
{
    Vector3 hitpt(0,0,0);
    Vector3 viewRay = viewRayFromScreenCoordinate(vx, vy);
    
    //  view ray intersecting with view z plane at 0,0,z
    ckm::plane3 zplane;
    zplane.normal.set(0,0,-1);
    zplane.pt.set(0,0,z);
    ckm::raytest<float>::planeIntersection(&hitpt, Vector3::kZero, viewRay, zplane);
    
    return hitpt;
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