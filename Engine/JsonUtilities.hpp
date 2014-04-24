//
//  JsonUtilities.hpp
//  Overview
//
//  Created by Samir Sinha on 4/22/14
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_JsonUtilities_hpp
#define Overview_JsonUtilities_hpp

#include "rapidjson/document.h"
#include <cstdlib>

namespace cinekine { namespace ovengine {

    typedef rapidjson::GenericDocument<rapidjson::UTF8<> > JsonDocument;
    typedef rapidjson::GenericValue<rapidjson::UTF8<> > JsonValue;

    int32_t parseInt(const JsonValue& value, int32_t defaultVal=0);
    uint32_t parseUint(const JsonValue& value, uint32_t defaultVal=0);

} /* namespace ovengine */ } /* namespace cinekine */

#endif
