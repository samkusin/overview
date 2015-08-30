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
#include "Engine/EngineGeometry.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>

namespace ckm {

//  Right Handed Coordinate System where view -Z is far, +Z is near
//  Forward in model space is +Z
//      - If the camera faces an object head on, down the Z axis, the object's
//        "forward" direction faces the camera.
//      - If an object moves forward towards another object, it will move +Z in
//        model space (i.e. moves closer)
//      - Moving backwards, or -Z in model space means the object move's farther
//        away down -Z


    inline ckm::scalar epsilon() {
        return glm::epsilon<ckm::scalar>();
    }

    inline ckm::vec3 cross(vec3 const& x, vec3 const& y) {
        return glm::cross(x,y);
    }
    
    template<typename _Vector>
    typename _Vector::value_type dot(_Vector const& x, _Vector const& y) {
        return glm::dot(x,y);
    }
    
    inline scalar cos(scalar r) {
        return glm::cos(r);
    }
    inline scalar acos(scalar r) {
        return glm::acos(r);
    }
    inline scalar sin(scalar r) {
        return glm::sin(r);
    }
    inline scalar asin(scalar r) {
        return glm::asin(r);
    }
    inline scalar radians(scalar r) {
        return glm::radians(r);
    }
    inline scalar degrees(scalar r) {
        return glm::degrees(r);
    }
    
    template<typename _Matrix>
    _Matrix inverse(_Matrix const& m) {
        return glm::inverse(m);
    }
    
    mat4 mtx4x4RotateFromAngleAndAxis(mat4 const& m, scalar angle, vec3 const& axis);
    
    template<typename _Vector>
    _Vector normalize(_Vector const& v) {
        return glm::normalize(v);
    }
    
    template<typename _Vector>
    scalar vectorLength(_Vector const& v) {
        return glm::length(v);
    }
    
    inline quat normalize(quat const& q) {
        return glm::normalize(q);
    }
    
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
    
    inline scalar pi() {
        return glm::pi<scalar>();
    }
    
    template<typename _T>
    inline _T zero() {
        return glm::zero<_T>();
    }
    
    //  Referemce
    //  lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors
    //  Inputs must be normalized (unit vectors)
    //  The returned quaternion is normalized.
    //
    quat quatFromUnitVectors(vec3 const& v0, vec3 const& v1);
    
}

#endif
