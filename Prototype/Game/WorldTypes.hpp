//
//  WorldTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_Model_WorldTypes_hpp
#define Overview_Game_Model_WorldTypes_hpp

#include "Engine/Model/ModelTypes.hpp"
#include "Engine/Model/AABB.hpp"
#include "LinearMath/btVector3.h"

namespace cinek { namespace overview {

    inline glm::vec3 toVec3(const btVector3& btv3) {
        glm::vec3 ret(btv3.x(), btv3.y(), btv3.z());
        return ret;
    }

    inline btVector3 toBtVector3(const glm::vec3& vec3) {
        btVector3 ret(vec3.x, vec3.y, vec3.z);
        return ret;
    }

    using WorldPoint = overview::Point;
    using WorldAABB = overview::AABB<WorldPoint>;


} /* namespace overview */ } /* namespace overview */

#endif
