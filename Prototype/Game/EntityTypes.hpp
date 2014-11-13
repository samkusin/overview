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

#include "Game/WorldTypes.hpp"
#include "cinek/value_map.hpp"
#include "cinek/string.hpp"

#include <glm/glm.hpp>

namespace cinek {
    namespace overview {

    using EntityId = WorldObjectId;

    using EntityCommand = ValueMap<int, Value<std::string, glm::vec3>>;
    
    enum
    {
        kEntityResultFlag_Activated    = 0x00000001,
        kEntityResultFlag_Position     = 0x00000002,
        kEntityResultFlag_Direction    = 0x00000004,
        kEntityResultFlag_Deactivated  = 0x00000008,
        kEntityResultFlag_Destroyed    = 0x80000000
    };

    } /* ovengine */
} /* cinekine */

#endif