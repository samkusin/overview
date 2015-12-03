/**
 * @file        Scene.hpp
 * @author      Samir Sinha
 * @date        11/20/2015
 * @brief       Definitions for the Scene class
 * @copyright   Copyright 2015 Cinekine.  
 *
 */

#ifndef Overview_Scene_hpp
#define Overview_Scene_hpp

#include "Engine/EngineTypes.hpp"

#include <cinek/allocator.hpp>

namespace cinek {
    namespace ove {
/**
 *  @class  Scene
 *  @brief  A Scene represents the physical world of a simulation.
 *
 *  The Scene applies the physical components for an Entity.  Its interface
 *  abstracts the details (such as a Rigid Body Physics engine.)  Controllers
 *  place entities within a Scene for simulation.
 *
 */
template<typename Impl>
class Scene
{
public:
    Scene();
    ~Scene();
    
    //void loadManifest(const char* path
    
    /**
     *  Updates the scene simulations using the specified timestep.  It's best
     *  to run this using a fixed timestep, though this requirement depends
     *  on the needs of an application.
     *
     *  @param  dt      Time step in seconds
     */
    void simulate(double dt);
    
private:
    unique_ptr<Impl> _impl;
};
    
    } /* namespace ove */
} /* namespace cinek  */

#endif /* Scene_hpp */
