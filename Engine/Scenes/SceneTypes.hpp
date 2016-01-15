//
//  SceneTypes.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/6/16.
//
//

#ifndef Overview_SceneTypes_hpp
#define Overview_SceneTypes_hpp

#include "Engine/EngineTypes.hpp"

#include <bullet/LinearMath/btVector3.h>
#include <bullet/LinearMath/btMatrix3x3.h>
#include <bullet/LinearMath/btQuaternion.h>

class btRigidBody;
class btBvhTriangleMeshShape;

namespace cinek {
    namespace ove {

struct SceneBody
{
    btRigidBody* btBody = nullptr;
    Entity entity = 0;
    
    struct SavedState
    {
        enum
        {
            kNone           = 0x0000,
            kDynamic        = 0x0001
        };
        
        uint32_t flags;
        btScalar mass;
        btVector3 inertia;
    };
    
    SavedState savedState;
    
    void activate();
    void deactivate();
};
   
struct SceneRayTestResult
{
    operator bool() const { return body != nullptr; }
    
    btRigidBody* body;
    Entity entity;
    btVector3 normal;
    btVector3 position;
};
      
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_SceneTypes_hpp */
