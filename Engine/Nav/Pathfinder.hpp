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
    
    //  Manages listeners for Pathfinder events
    void addListener(PathfinderListener* listener);
    void removeListener(PathfinderListener* listener);
    
    //  checks whether a world location is walkable.  this should be used as a
    //  quick way to determine whether a target can be used as a destination,
    //  though this method does not check if an entity can reach the target
    //  position.
    bool isLocationWalkable(ckm::vector3f pos, ckm::vector3f extents);
    
    //  sends a request to generate a path between two points.
    //  paths are sent to listeners
    void generatePath(Entity entity, ckm::vector3f startPos, ckm::vector3f endPos);

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
