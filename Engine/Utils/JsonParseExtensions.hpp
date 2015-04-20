//
//  JsonParseExtensions.hpp
//  Overview
//
//  Created by Samir Sinha on 4/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Utils_JsonParse_hpp
#define Overview_Utils_JsonParse_hpp

#include "Engine/Defines.hpp"
#include <cinek/json/json.hpp>

#include <glm/glm.hpp>

namespace cinek { namespace overview {

glm::vec3 parseVec3(const JsonValue& value, const glm::vec3& defaultVal=glm::vec3());

} /* namespace overview */ } /* namespace cinek */

#endif
