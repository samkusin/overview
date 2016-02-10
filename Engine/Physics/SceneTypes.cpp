//
//  SceneTypes.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/6/16.
//
//

#include "SceneTypes.hpp"

#include "SceneFixedBodyHull.hpp"

#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>
#include <bullet/BulletCollision/CollisionShapes/btCollisionShape.h>
#include <bullet/BulletCollision/CollisionShapes/btTriangleMeshShape.h>

namespace cinek {
    namespace ove {
  

const SceneFixedBodyHull* SceneBody::getFixedHull() const
{
    const SceneFixedBodyHull* hull = nullptr;
    if (btBody) {
        const btCollisionShape* shape = btBody->getCollisionShape();
        if (shape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE) {
            const btTriangleMeshShape* triMeshShape =
                static_cast<const btTriangleMeshShape*>(shape);
            hull = static_cast<const SceneFixedBodyHull*>(triMeshShape->getMeshInterface());
        }
    }
    return hull;
}
  
  
    } /* namespace ove */
} /* namespace cinek */
