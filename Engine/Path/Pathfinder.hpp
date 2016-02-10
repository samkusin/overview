//
//  Pathfinder.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/9/16.
//
//

#ifndef Overview_Pathfinder_hpp
#define Overview_Pathfinder_hpp

#include "Engine/EngineTypes.hpp"

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
    
    //  update the pathfinding system
    void update(double dt);

private:
    class Impl;
    unique_ptr<Impl> _impl;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_Pathfinder_hpp */
