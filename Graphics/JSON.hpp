
#ifndef CK_Graphics_JSON_hpp
#define CK_Graphics_JSON_hpp

#include "Stream/StreamBufRapidJson.hpp"
#include "rapidjson/document.h"

namespace cinekine {
    namespace glx {

    typedef rapidjson::GenericDocument<rapidjson::UTF8<> > Document;
    typedef rapidjson::GenericValue<rapidjson::UTF8<> > Value;

    }   /* namespace glx */
}  /* namespace cinekine */

#endif
