//
//  EngineGeometry.hpp
//  Overview
//
//  Created by Samir Sinha on 7/6/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Engine_Geometry_hpp
#define Overview_Engine_Geometry_hpp

#include "Engine/EngineMathTypes.hpp"

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <array>


namespace ckm {

    struct Plane3
    {
        using value_type = ckm::vec3;
        value_type normal;
        value_type pt;
        
        scalar testPoint(const value_type& testPt) const;
    };
    
    class Frustrum
    {
    public:
        enum Plane
        {
            kNearZ,
            kFarZ,
            kLeftX,
            kRightX,
            kTopY,
            kBottomY,
            kPlaneCount
        };
    
        using Shell = std::array<Plane3, kPlaneCount>;
        
        Frustrum();
        Frustrum(scalar nearZ, scalar farZ, scalar fov, scalar aspect);
    
        scalar nearZ() const { return _nearZ; }
        scalar farZ() const { return _farZ; }
        scalar fovRadians() const { return _fovRadians; }
        scalar aspect() const { return _aspect; }
        
        const Shell& shell() const { return _shell; }
        
        Frustrum transform(const mat3& basis, const vec3& translate) const;
        
        bool testAABB(const AABB<Plane3::value_type>& aabb) const;
        bool testAABBWithPlane(const AABB<Plane3::value_type>& aabb, Plane plane) const;
        
    private:
        Shell _shell;
        scalar _nearZ, _farZ;
        scalar _aspect;
        scalar _fovRadians;
    };
    
}

#endif
