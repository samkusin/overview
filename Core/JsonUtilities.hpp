//
//  JsonUtilities.hpp
//  Overview
//
//  Created by Samir Sinha on 4/22/14
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef CK_Core_JsonUtilities_hpp
#define CK_Core_JsonUtilities_hpp

#include <rapidjson/document.h>
#include <glm/glm.hpp>
#include <cstdlib>


namespace cinekine {

    typedef rapidjson::GenericDocument<rapidjson::UTF8<> > JsonDocument;
    typedef rapidjson::GenericValue<rapidjson::UTF8<> > JsonValue;

    int32_t parseInt(const JsonValue& value, int32_t defaultVal=0);
    uint32_t parseUint(const JsonValue& value, uint32_t defaultVal=0);

    glm::vec3 parseVec3(const JsonValue& value, const glm::vec3& defaultVal=glm::vec3());

    uint32_t parseFlagsToUint(const JsonValue& flagsDef, const char* flags);

} /* namespace cinekine */

#endif
