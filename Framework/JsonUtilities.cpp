//
//  JsonUtilities.cpp
//  Overview
//
//  Created by Samir Sinha on 4/22/14
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "JsonUtilities.hpp"

namespace cinekine {

    int32_t parseInt(const JsonValue& value, int32_t defaultVal)
    {
        if (value.IsString())
            return strtol(value.GetString(), NULL, 0);
        else if (value.IsInt())
            return value.GetInt();
        return defaultVal;
    }
    uint32_t parseUint(const JsonValue& value, uint32_t defaultVal)
    {
        if (value.IsString())
            return strtoul(value.GetString(), NULL, 0);
        else if (value.IsUint())
            return value.GetUint();
        return defaultVal;
    }

} /* namespace cinekine */
