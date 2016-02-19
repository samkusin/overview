//
//  Pathfinder.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/9/16.
//
//

#ifndef Overview_Nav_Pathfinder_hpp
#define Overview_Nav_Pathfinder_hpp

#include "PathTypes.hpp"

#include <cinek/allocator.hpp>
#include <functional>

namespace cinek {
    namespace ove {
    
class Pathfinder
{
public:
    Pathfinder();
    ~Pathfinder();

    //  generates pathfinding data from an input scene
    using GenerateCb = std::function<void(bool)>;
    void generateFromScene(const Scene& scene, GenerateCb callback);
    
    //  checks whether a world location is walkable.  this should be used as a
    //  quick way to determine whether a target can be used as a destination,
    //  though this method does not check if an entity can reach the target
    //  position.
    bool isLocationWalkable(ckm::vector3f pos, ckm::vector3f extents);
    
    //  sends a request to an entity to move to the specified destination.  if
    //  unreachable, then notfies any listeners about the error.  from the
    //  caller's point of view, if it hasn't attached any listeners, this is
    //  a fire-and-forget op
    void entityGotoPosition(Entity entity, ckm::vector3f pos);
    
    //  update the pathfinding system
    void update(double dt);
    //  updates the debugging system for pathfinding
    void updateDebug(PathfinderDebug& debugger);
    
private:
    class Impl;
    unique_ptr<Impl> _impl;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_Nav_Pathfinder_hpp */
