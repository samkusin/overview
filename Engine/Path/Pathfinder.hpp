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
    
    //  cancels commands by listener
    void cancelByListener(PathfinderListener* listener);

    //  generates pathfinding data from an input scene
    using GenerateCb = std::function<void(bool)>;
    void generateFromScene(const Scene& scene, GenerateCb callback);
    
    NavPathQueryPtr acquireQuery();
    
    //  sends a request to generate a path between two points.
    //  paths are sent to listeners
    void generatePath
    (
        PathfinderListener* target,
        Entity entity,
        ckm::vector3f startPos,
        ckm::vector3f endPos
    );

    //  update the pathfinding system
    void simulate(double dt);
    //  updates the debugging system for pathfinding
    void simulateDebug(PathfinderDebug& debugger);
    
private:
    class Impl;
    unique_ptr<Impl> _impl;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_Nav_Pathfinder_hpp */
