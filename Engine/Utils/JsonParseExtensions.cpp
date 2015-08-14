//
//  JsonParseExtensions.cpp
//  Overview
//
//  Created by Samir Sinha on 4/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "JsonParseExtensions.hpp"
#include "Engine/EngineMath.hpp"

namespace cinek { namespace overview {

    ckm::vec3 parseVec3(const JsonValue& value, const glm::vec3& defaultVal)
    {
        if (!value.IsObject()) 
            return defaultVal;
        glm::vec3 v = defaultVal;
        auto& vx = value["x"];
        auto& vy = value["y"];
        auto& vz = value["z"];
        if (vx.IsDouble()) v.x = vx.GetDouble();
        if (vy.IsDouble()) v.y = vy.GetDouble();
        if (vz.IsDouble()) v.z = vz.GetDouble();
        return v;
    }
    
} /* namespace overview */ } /* namespace cinek */
