//
//  PathfinderListener.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/19/16.
//
//

#ifndef Overview_Nav_PathfinderListener_hpp
#define Overview_Nav_PathfinderListener_hpp

#include "PathTypes.hpp"

namespace cinek {
    namespace ove {
    
class PathfinderListener
{
public:
    virtual ~PathfinderListener() {}
    
    virtual void onPathfinderPathUpdate(Entity entity, NavPath&& path) = 0;
    
    virtual void onPathfinderError(Entity entity, PathfinderError error) = 0;
};

    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_Nav_PathfinderListener_hpp */
