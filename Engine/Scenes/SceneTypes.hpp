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
    btRigidBody* btBody;
    Entity entity;
};
   
struct SceneRayTestResult
{
    SceneRayTestResult() : body { nullptr, 0 } {}
    operator bool() const { return body.btBody != nullptr; }
    
    SceneBody body;
    btVector3 normal;
    btVector3 position;
};
      
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_SceneTypes_hpp */
