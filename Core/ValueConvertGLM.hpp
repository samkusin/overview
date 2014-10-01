//
//  ValueConvertGLM.hpp
//  Overview
//
//  Created by Samir Sinha on 9/29/14
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef CK_Core_ValueConvertGLM_hpp
#define CK_Core_ValueConvertGLM_hpp

#include "cinek/value_convert.hpp"

#include <glm/glm.hpp>
#include <cstdlib>


namespace cinekine {

template<> class value_convert<glm::ivec2, glm::ivec2> {
public:
    static glm::ivec2 value(const glm::ivec2& src) {
        return src;
    }
};

template<> class value_convert<glm::vec3, glm::vec3> {
public:
    static glm::vec3 value(const glm::vec3& src) {
       return src;
    }
};

} /* namespace cinekine */

#endif
