//
//  RecastContext.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/4/16.
//
//

#ifndef Overview_Utils_RecastContext_hpp
#define Overview_Utils_RecastContext_hpp

#include "PathTypes.hpp"
#include "Engine/Contrib/Recast/Recast.h"


namespace cinek {
    namespace ove {
    
    class RecastContext : public ::rcContext
    {
    public:
        RecastContext();
    
    protected:
    	virtual void doLog
        (
            const rcLogCategory category,
            const char* msg,
            const int /*len*/
        );
    };
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* RecastContext_hpp */
