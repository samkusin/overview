//
//  EngineGeometry.cpp
//  Overview
//
//  Created by Samir Sinha on 7/6/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "EngineGeometry.hpp"
#include "EngineMath.hpp"
#include "AABB.hpp"

namespace ckm {

ckm::scalar Plane3::testPoint(const value_type& testPt) const
{
    auto ptv = testPt - pt;
    return ckm::dot(ptv, normal);
}


Frustrum::Frustrum() :
    _nearZ(0),
    _farZ(0),
    _aspect(0)
{
}

Frustrum::Frustrum
(
    ckm::scalar nearZ,
    ckm::scalar farZ,
    ckm::scalar fov,
    ckm::scalar aspect
) :
    _nearZ(nearZ),
    _farZ(farZ),
    _aspect(aspect),
    _fovRadians(fov)
{
    auto fovTan2 = 2 * std::tan(_fovRadians/2);
    auto nearH = fovTan2 * nearZ;
    auto nearW = nearH * aspect;
    auto farH = fovTan2 * farZ;
    auto farW = farH * aspect;
    
    auto nearCenter = ckm::vec3(0,0,1) * nearZ;
    auto farCenter = ckm::vec3(0,0,1) * farZ;

    auto nearTL = nearCenter + (ckm::vec3(0,1,0) * (nearH/2)) + (ckm::vec3(-1,0,0) * (nearW/2));
    auto nearBL = nearCenter + (ckm::vec3(0,-1,0) * (nearH/2)) + (ckm::vec3(-1,0,0) * (nearW/2));
    auto farTL = farCenter + (ckm::vec3(0,1,0) * (farH/2)) + (ckm::vec3(-1,0,0) * (farW/2));
    
    _shell[kLeftX].normal = ckm::normalize(ckm::cross(farTL - nearTL, nearBL - nearTL));
    _shell[kLeftX].pt = nearTL;
    
    auto nearTR = nearCenter + (ckm::vec3(0,1,0) * (nearH/2)) + (ckm::vec3(1,0,0) * (nearW/2));
    auto nearBR = nearCenter + (ckm::vec3(0,-1,0) * (nearH/2)) + (ckm::vec3(1,0,0) * (nearW/2));
    auto farTR = farCenter + (ckm::vec3(0,1,0) * (farH/2)) + (ckm::vec3(1,0,0) * (farW/2));

    _shell[kRightX].normal = ckm::normalize(ckm::cross(nearBR - nearTR, farTR - nearTR));
    _shell[kRightX].pt = nearTR;
    
    _shell[kTopY].normal = ckm::normalize(ckm::cross(farTR - nearTR, nearTL - nearTR));
    _shell[kTopY].pt = farTR;
    
    auto farBL = farCenter + (ckm::vec3(0,-1,0) * (farH/2)) + (ckm::vec3(-1,0,0) * (farW/2));
    auto farBR = farCenter + (ckm::vec3(0,-1,0) * (farH/2)) + (ckm::vec3(1,0,0) * (farW/2));
    
    _shell[kBottomY].normal = ckm::normalize(ckm::cross(farBR - farBL, nearBL - farBL));
    _shell[kBottomY].pt = farBL;
    
    //  near and far planes have trivial normals
    _shell[kNearZ].normal = ckm::vec3(0,0,1);
    _shell[kNearZ].pt = nearCenter;
    _shell[kFarZ].normal = ckm::vec3(0,0,-1);
    _shell[kFarZ].pt = farCenter;
}


Frustrum Frustrum::transform
(
    const ckm::mat3& basis,
    const ckm::vec3& translate
)
const
{
    Frustrum frustrum;
    
    frustrum._aspect = _aspect;
    frustrum._fovRadians = _fovRadians;
    frustrum._nearZ = _nearZ;
    frustrum._farZ = _farZ;
    
    struct Fn
    {
        const ckm::mat3* basis;
        const ckm::vec3* translate;
        
        Plane3 operator()(const Plane3& pl) const {
            Plane3 ret = { *basis * pl.normal, *basis * pl.pt };
            ret.pt += *translate;
            return ret;
        }
    };
    Fn fn = { &basis, &translate };
    
    //  rotate our normals and points.  translate our points
    std::transform(_shell.begin(), _shell.end(), frustrum._shell.begin(), fn);
    
    return frustrum;
}


bool Frustrum::testAABB(const AABB<Plane3::value_type>& aabb) const
{
    if (_nearZ == _farZ)
        return false;
    
    // http://zach.in.tu-clausthal.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/index.html
    bool intersect = testAABBWithPlane(aabb, kNearZ) &&
                testAABBWithPlane(aabb,kFarZ) &&
                testAABBWithPlane(aabb,kLeftX) &&
                testAABBWithPlane(aabb,kRightX) &&
                testAABBWithPlane(aabb,kTopY) &&
                testAABBWithPlane(aabb,kBottomY);
   
    return intersect;
}

bool Frustrum::testAABBWithPlane
(
    const AABB<Plane3::value_type>& aabb,
    Plane planeType
)
const
{
    bool intersect = true;
    auto& plane = _shell[planeType];
    auto& normal = plane.normal;
        
    Plane3::value_type posV = aabb.min;
    if (normal.x >= 0)
        posV.x = aabb.max.x;
    if (normal.y >= 0)
        posV.y = aabb.max.y;
    if (normal.z >= 0)
        posV.z = aabb.max.z;
    
    if (plane.testPoint(posV) < 0)
        return false;   // outside
    /*
    //  tests if the box intersects - if we need to differentiate between
    //  intersect and insde, then uncomment this code and return the
    //  appropriate result.
    //
    Plane3::value_type negV = aabb.max;
    if (normal.x >= 0)
        negV.x = aabb.min.x;
    if (normal.y >= 0)
        negV.y = aabb.min.y;
    if (normal.z >= 0)
        negV.z = aabb.min.z;
    
    if (plane.testPoint(negV) < 0)
        intersect = true;
    */
    
    return intersect;
}

    
}