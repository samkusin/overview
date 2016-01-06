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

    Matrix4 worldMtx;
    Matrix4 viewMtx;
    Matrix4 projMtx;
    Matrix4 invProjMtx;
    Frustrum viewFrustrum;
};

    }   // namespace gfx
}   // namespace cinek



#endif /* Camera_h */
