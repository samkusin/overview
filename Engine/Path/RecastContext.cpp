//
//  RecastContext.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/4/16.
//
//

#include "RecastContext.hpp"
#include "Engine/Debug.hpp"

namespace cinek {
    namespace ove {
    
    RecastContext::RecastContext()
    {
    }
        
    void RecastContext::doLog
    (
        const rcLogCategory category,
        const char* msg,
        const int /*len*/
    )
    {
        if (category == RC_LOG_PROGRESS) {
            OVENGINE_LOG_INFO(msg);
        }
        else if (category == RC_LOG_WARNING) {
            OVENGINE_LOG_WARN(msg);
        }
        else if (category == RC_LOG_ERROR) {
            OVENGINE_LOG_ERROR(msg);
        }
    }

    } /* namespace ove */
} /* namespace cinek */