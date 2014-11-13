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

    using WorldObjectId = uint32_t;
    
    const btVector3 kWorldRefDir = btVector3(0,1,0);

    //  Utilities used for converting between coordinate systems
    inline Point toPoint(const btVector3& btv3) {
        Point ret(btv3.x(), btv3.y(), btv3.z());
        return ret;
    }

    inline btVector3 toBtVector3(const Point& vec3) {
        btVector3 ret(vec3.x, vec3.y, vec3.z);
        return ret;
    }

} /* namespace overview */ } /* namespace overview */

#endif
