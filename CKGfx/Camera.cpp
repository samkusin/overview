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
    
    bx::mtxProj(projMtx,
        180.0f * viewFrustrum.fovRadians()/bx::pi,
        viewFrustrum.aspect(),
        viewFrustrum.nearZ(),
        viewFrustrum.farZ(),
        false);
    
    bx::mtxInverse(invProjMtx, projMtx);
}

    }   /* namespace gfx */
}   /* namespace cinek */