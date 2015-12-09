//
//  EngineMath.hpp
//  Overview
//
//  Created by Samir Sinha on 7/6/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Engine_Math_hpp
#define Overview_Engine_Math_hpp

#include "Engine/EngineMathTypes.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>

namespace ckm {
    
    //  Referemce
    //  lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors
    //  Inputs must be normalized (unit vectors)
    //  The returned quaternion is normalized.
    //
    quat quatFromUnitVectors(vec3 const& v0, vec3 const& v1);
    
    mat4 mtx4x4RotateFromAngleAndAxis
    (
        mat4 const& m,
        scalar angle,
        vec3 const& axis
    );
    
    inline quat inverse(quat const& q) {
        return glm::inverse(q);
    }
    
    inline mat4 mtx4x4FromQuat(quat const& q) {
        return glm::mat4_cast(q);
    }
    
    inline mat3 mtx3x3FromQuat(quat const& q) {
        return glm::mat3_cast(q);
    }
    
    inline quat quatFromMtx4x4(mat4 const& m) {
        return glm::quat_cast(m);
    }
    
    inline quat quatFromMtx3x3(mat3 const& m) {
        return glm::quat_cast(m);
    }
    
    inline quat quatFromAngleAndAxis(scalar angle, vec3 const& axis) {
        return glm::angleAxis(angle, axis);
    }
    
    inline vec3 axisFromQuat(quat const& q) {
        return glm::axis(q);
    }
    
    inline scalar angleFromQuat(quat const& q) {
        return glm::angle(q);
    }
    

    
}

#endif
