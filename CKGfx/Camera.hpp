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
    void update();

    const Frustrum& viewFrustrum() const { return _viewFrustrum; }
    Vector3 worldPosition() const;
    Vector3 rayFromViewportCoordinate(const int32_t x, const int32_t y) const;
    
    Matrix4 worldMtx;
    Matrix4 viewMtx;
    Matrix4 projMtx;
    Matrix4 invProjMtx;
    
    gfx::Rect viewportRect;
    float near;
    float far;
    float fovDegrees;

private:
    Frustrum _viewFrustrum;
};

    }   // namespace gfx
}   // namespace cinek



#endif /* Camera_h */
