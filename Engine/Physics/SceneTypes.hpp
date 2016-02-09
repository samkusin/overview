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

class btTransform;
class btCollisionObject;
class btBvhTriangleMeshShape;

namespace cinek {
    namespace ove {

struct SceneBody
{
    btCollisionObject* btBody = nullptr;
    SceneMotionState* motionState = nullptr;
    Entity entity = 0;
    
    enum
    {
        kSection            = 0x01,
        kObject             = 0x02
    };
    
    enum
    {
        kIsSection          = 1 << kSection,
        kIsObject           = 1 << kObject,
        kAllCategories      = 0xffffffff
    };
    
    uint32_t categoryMask = 0;
};
   
struct SceneRayTestResult
{
    explicit operator bool() const { return body != nullptr; }
    
    SceneBody* body = nullptr;
    btVector3 normal;
    btVector3 position;
};
      
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_SceneTypes_hpp */
