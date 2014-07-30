//
//  AABB.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Model/AABB.hpp"

#include <algorithm>

namespace cinekine { namespace ovengine {

    template<class _Point>
    AABB<_Point> AABB<_Point>::boundTo(const AABB<_Point>& box) const
    {
        AABB<_Point> result;
        result.min.x = std::min(box.min.x, min.x);
        result.min.y = std::min(box.min.y, min.y);
        result.min.z = std::min(box.min.z, min.z);
        result.max.x = std::max(box.max.x, max.x);
        result.max.y = std::max(box.max.y, max.y);
        result.max.z = std::max(box.max.z, max.z);
        return result;
    }

} /* namespace ovengine */ } /* namespace cinekine */
