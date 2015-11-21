//
//  Scene.inl
//  EnginePrototype
//
//  Created by Samir Sinha on 11/20/15.
//
//

#include "Scene.hpp"

namespace cinek {
    namespace ove {
    

////////////////////////////////////////////////////////////////////////////////

template<typename Impl>
Scene<Impl>::Scene()
{
    Allocator allocator;
    _impl = allocate_unique<Impl>(allocator);
}

template<typename Impl>
Scene<Impl>::~Scene() = default;
 
template<typename Impl>
void Scene<Impl>::simulate(double dt)
{
    _impl->simulate(dt);
}
        
    }   /* namespace ove */
}   /* namespace cinek */