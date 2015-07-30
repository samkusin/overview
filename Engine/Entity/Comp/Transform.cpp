//
//  Transform.cpp
//  Overview
//
//  Created by Samir Sinha on 6/26/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "Transform.hpp"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace cinek { namespace overview {

namespace component
{
    COMPONENT_TYPE_IMPL(Transform, kTransform, {
        _isDirty = false;
        _worldSRT = glm::dmat4(1.0);
    });
    
    void Transform::calculateWorldSRTFromParent(const glm::dmat4& parent)
    {
        if (!_isDirty)
            return;
            
        glm::dmat4 local = glm::dmat4(1.0);
        local[3] = glm::dvec4(_localPosition, 1.0);
 
        _worldSRT = local * parent;
        
        _isDirty = false;
    }
}

} /* namespace overview */ } /* namespace cinek */
