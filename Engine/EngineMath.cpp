//
//  EngineMath.cpp
//  Overview
//
//  Created by Samir Sinha on 8/25/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "EngineMath.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace ckm
{
    quat quatFromUnitVectors(vec3 const& v0, vec3 const& v1)
    {
        vec3 w = cross(v0, v1);
        quat q = quat(1 + dot(v0, v1), w.x,w.y,w.z);
        return normalize(q);
    }
    
    mat4 mtx4x4RotateFromAngleAndAxis
    (
        mat4 const& m,
        scalar angle,
        vec3 const& axis
    )
    {
        return glm::rotate(m, angle, axis);
    }
}
