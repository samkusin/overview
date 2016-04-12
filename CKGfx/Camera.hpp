//
//  Camera.hpp
//  GfxPrototype
//
//  Created by Samir Sinha on 10/7/15.
//
//

#ifndef CK_Graphics_Camera_hpp
#define CK_Graphics_Camera_hpp

#include "GfxTypes.hpp"

#include <ckm/geometry.hpp>

namespace cinek {
    namespace gfx {

struct Camera
{
    enum class Mode
    {
        kPerspective,
        kOrthogonal
    };
    
    Camera() : viewIndex(0), _viewMode(Mode::kPerspective) {}

    void setMode(Mode mode) { _viewMode = mode; }
    Mode mode() const { return _viewMode; }
    
    void update();

    const Frustrum& viewFrustrum() const { return _viewFrustrum; }
    Vector3 worldPosition() const;
    Vector4 viewTranslateFromScreenCoordinate(const int32_t vx, const int32_t vy) const;
    Vector3 viewRayFromScreenCoordinate(const int32_t vx, const int32_t vy) const;
    Vector3 viewPositionFromScreenCoordinate(const int32_t vx, const int32_t vy, const float z) const;
    Vector3 worldRayFromScreenCoordinate(const int32_t x, const int32_t y) const;
    Vector2 worldToScreenCoordinates(const Vector4& worldPos, bool* onscreen=nullptr) const;
    
    Matrix4 worldMtx;
    Matrix4 viewMtx;
    Matrix4 projMtx;
    Matrix4 invProjMtx;
    Matrix4 viewProjMtx;
    
    gfx::Rect viewportRect;
    float near;
    float far;
    float fovDegrees;
    
    int viewIndex;

private:
    Frustrum _viewFrustrum;
    Mode _viewMode;
};

    }   // namespace gfx
}   // namespace cinek



#endif /* Camera_h */
