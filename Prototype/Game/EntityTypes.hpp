///
/// @file
/// Utilities for an Axis Aligned Bounding Box (AABB)
///
/// @author     Samir Sinha
/// @date       09/23/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Model_EntityTypes_hpp
#define Overview_Model_EntityTypes_hpp

#include "cinek/value_map.hpp"
#include "cinek/string.hpp"

#include <glm/glm.hpp>

namespace cinek {
    namespace overview {

    using EntityId = uint32_t;

    using EntityCommand = ValueMap<int, Value<std::string, glm::vec3>>;

    } /* ovengine */
} /* cinekine */

#endif