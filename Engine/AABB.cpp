//
//  AABB.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/AABB.hpp"

#include <glm/glm.hpp>
#include <algorithm>

namespace cinek { namespace overview {

    template<>
    bool AABB<glm::ivec2>::inside(const AABB<glm::ivec2>& box) const
    {
        return (min.x >= box.min.x && max.x <= box.max.x &&
                min.y >= box.min.y && max.y <= box.max.y);
    }
    template<>
    bool AABB<glm::vec2>::inside(const AABB<glm::vec2>& box) const
    {
        return (min.x >= box.min.x && max.x <= box.max.x &&
                min.y >= box.min.y && max.y <= box.max.y);
    }
    template<>
    bool AABB<glm::ivec3>::inside(const AABB<glm::ivec3>& box) const
    {
        return (min.x >= box.min.x && max.x <= box.max.x &&
                min.y >= box.min.y && max.y <= box.max.y &&
                min.z >= box.min.z && max.z <= box.max.z);
    }
    template<>
    bool AABB<glm::vec3>::inside(const AABB<glm::vec3>& box) const
    {
        return (min.x >= box.min.x && max.x <= box.max.x &&
                min.y >= box.min.y && max.y <= box.max.y &&
                min.z >= box.min.z && max.z <= box.max.z);
    }

    template<>
    bool AABB<glm::ivec2>::contains(const point_type& pt) const
    {
        return (pt.x >= min.x && pt.x <= max.x &&
                pt.y >= min.y && pt.y <= max.y);
    }
    template<>
    bool AABB<glm::vec2>::contains(const point_type& pt) const
    {
        return (pt.x >= min.x && pt.x <= max.x &&
                pt.y >= min.y && pt.y <= max.y);
    }
    template<>
    bool AABB<glm::ivec3>::contains(const point_type& pt) const
    {
        return (pt.x >= min.x && pt.x <= max.x &&
                pt.y >= min.y && pt.y <= max.y &&
                pt.z >= min.z && pt.z <= max.z);
    }
    template<>
    bool AABB<glm::vec3>::contains(const point_type& pt) const
    {
        return (pt.x >= min.x && pt.x <= max.x &&
                pt.y >= min.y && pt.y <= max.y &&
                pt.z >= min.z && pt.z <= max.z);
    }

    template<>
    bool AABB<glm::ivec2>::outside(const AABB<glm::ivec2>& box) const
    {
        return (min.x > box.max.x || box.min.x > max.x ||
                min.y > box.max.y || box.min.y > max.y);
    }
    template<>
    bool AABB<glm::vec2>::outside(const AABB<glm::vec2>& box) const
    {
        return (min.x > box.max.x || box.min.x > max.x ||
                min.y > box.max.y || box.min.y > max.y);
    }
    template<>
    bool AABB<glm::ivec3>::outside(const AABB<glm::ivec3>& box) const
    {
        return (min.x > box.max.x || box.min.x > max.x ||
                min.y > box.max.y || box.min.y > max.y ||
                min.z > box.max.z || box.min.z > max.z);
    }
    template<>
    bool AABB<glm::vec3>::outside(const AABB<glm::vec3>& box) const
    {
        return (min.x > box.max.x || box.min.x > max.x ||
                min.y > box.max.y || box.min.y > max.y ||
                min.z > box.max.z || box.min.z > max.z);
    }

    template<>
    AABB<glm::ivec2> AABB<glm::ivec2>::boundTo(const AABB<glm::ivec2>& box) const
    {
        AABB<glm::ivec2> result;
        result.min.x = std::min(box.min.x, min.x);
        result.min.y = std::min(box.min.y, min.y);
        result.max.x = std::max(box.max.x, max.x);
        result.max.y = std::max(box.max.y, max.y);
        return result;
    }
    template<>
    AABB<glm::vec2> AABB<glm::vec2>::boundTo(const AABB<glm::vec2>& box) const
    {
        AABB<glm::vec2> result;
        result.min.x = std::min(box.min.x, min.x);
        result.min.y = std::min(box.min.y, min.y);
        result.max.x = std::max(box.max.x, max.x);
        result.max.y = std::max(box.max.y, max.y);
        return result;
    }
    template<>
    AABB<glm::ivec3> AABB<glm::ivec3>::boundTo(const AABB<glm::ivec3>& box) const
    {
        AABB<glm::ivec3> result;
        result.min.x = std::min(box.min.x, min.x);
        result.min.y = std::min(box.min.y, min.y);
        result.min.z = std::min(box.min.z, min.z);
        result.max.x = std::max(box.max.x, max.x);
        result.max.y = std::max(box.max.y, max.y);
        result.max.z = std::max(box.max.z, max.z);
        return result;
    }
    template<>
    AABB<glm::vec3> AABB<glm::vec3>::boundTo(const AABB<glm::vec3>& box) const
    {
        AABB<glm::vec3> result;
        result.min.x = std::min(box.min.x, min.x);
        result.min.y = std::min(box.min.y, min.y);
        result.min.z = std::min(box.min.z, min.z);
        result.max.x = std::max(box.max.x, max.x);
        result.max.y = std::max(box.max.y, max.y);
        result.max.z = std::max(box.max.z, max.z);
        return result;
    }


} /* namespace overview */ } /* namespace cinek */
