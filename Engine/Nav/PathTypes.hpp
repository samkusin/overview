//
//  PathTypes.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/11/16.
//
//

#ifndef Overview_Nav_PathTypes_hpp
#define Overview_Nav_PathTypes_hpp

#include "Engine/EngineTypes.hpp"

struct duDebugDraw;


namespace cinek {
    namespace ove {
    
    
struct RecastMeshConfig
{
    float walkableClimb;
    float walkableHeight;
    float walkableRadius;
    float cellSize;
    float cellHeight;
};

    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_PathTypes_hpp */
