//
//  EngineMath.hpp
//  Overview
//
//  Created by Samir Sinha on 7/6/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Engine_Math_hpp
#define Overview_Engine_Math_hpp

#include "EngineMathTypes.hpp"
#include "EngineGeometry.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace ckm {

    inline ckm::vec3 cross(ckm::vec3 const& x, ckm::vec3 const& y) {
        return glm::cross(x,y);
    }
    
    template<typename _Vector>
    typename _Vector::value_type dot(_Vector const& x, _Vector const& y) {
        return glm::dot(x,y);
    }
    
    template<typename _Matrix>
    _Matrix inverse(_Matrix const& m) {
        return glm::inverse(m);
    }
    
    template<typename _Vector>
    _Vector normalize(_Vector const& v) {
        return glm::normalize(v);
    }
    
}

#endif
