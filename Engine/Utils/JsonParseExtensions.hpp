//
//  JsonParseExtensions.hpp
//  Overview
//
//  Created by Samir Sinha on 4/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Utils_JsonParse_hpp
#define Overview_Utils_JsonParse_hpp

#include <cinek/types.hpp>
#include <cinek/json/json.hpp>

#include "Engine/EngineMathTypes.hpp"

namespace cinek { namespace overview {

ckm::vec3 parseVec3(const JsonValue& value, const glm::vec3& defaultVal=ckm::vec3());

} /* namespace overview */ } /* namespace cinek */

#endif
