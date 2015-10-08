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
    //  world transform matrix (with 'world' as defined for the rendered graph.)
    Matrix4 worldMtx;
    //  view frustrum relative to the current view matrix
    Frustrum viewFrustrum;
};

    }   // namespace gfx
}   // namespace cinek



#endif /* Camera_h */
