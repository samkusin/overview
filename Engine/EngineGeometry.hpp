//
//  EngineGeometry.hpp
//  Overview
//
//  Created by Samir Sinha on 7/6/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Engine_Geometry_hpp
#define Overview_Engine_Geometry_hpp

#include "EngineMathTypes.hpp"

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <array>


namespace ckm {

    struct Plane3
    {
        using value_type = ckm::vec3;
        value_type normal;
        value_type pt;
        
        ckm::scalar testPoint(const value_type& testPt) const;
    };
    
    class Frustrum
    {
    public:
        enum
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
        Frustrum(ckm::scalar nearZ, ckm::scalar farZ, ckm::scalar fov, ckm::scalar aspect);
    
        ckm::scalar nearZ() const { return _nearZ; }
        ckm::scalar farZ() const { return _farZ; }
        ckm::scalar fovRadians() const { return _fovRadians; }
        ckm::scalar aspect() const { return _aspect; }
        
        const Shell& shell() const { return _shell; }
        
        Frustrum transform(const ckm::mat3& basis, const ckm::vec3& translate) const;
        
        bool testAABB(const AABB<Plane3::value_type>& aabb) const;
        
    private:
        Shell _shell;
        ckm::scalar _nearZ, _farZ;
        ckm::scalar _aspect;
        ckm::scalar _fovRadians;
    };
    
}

#endif
