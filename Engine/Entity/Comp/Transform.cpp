//
//  Transform.cpp
//  Overview
//
//  Created by Samir Sinha on 6/26/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "Transform.hpp"
#include "Engine/EngineMath.hpp"

namespace cinek { namespace overview {

namespace component
{
    COMPONENT_TYPE_IMPL(Transform, kTransform, {
        _localOrient = ckm::quat(1,0,0,0);
        _orient = ckm::quat(1,0,0,0);
    });
    
    
    ckm::mat4& Transform::calcLocalMatrix(ckm::mat4& srt) const
    {
        srt = ckm::mtx4x4FromQuat(_localOrient);
        srt[3] = ckm::vec4(_localPosition, 1.0);
        return srt;
    }
    
    ckm::mat4& Transform::calcMatrix(ckm::mat4& srt) const
    {
        srt = ckm::mtx4x4FromQuat(_orient);
        srt[3] = ckm::vec4(_position, 1.0);
        return srt;
    }
    

    ckm::mat3& Transform::calcLocalBasis(ckm::mat3& basis) const
    {
        basis = ckm::mtx3x3FromQuat(_localOrient);
        return basis;
    }
    
    ckm::mat3& Transform::calcBasis(ckm::mat3& basis) const
    {
        basis = ckm::mtx3x3FromQuat(_orient);
        return basis;
    }
    
}

} /* namespace overview */ } /* namespace cinek */
