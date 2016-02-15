//
//  SceneTypes.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/6/16.
//
//

#include "SceneTypes.hpp"
#include "SceneFixedBodyHull.hpp"
#include "SceneMotionState.hpp"

#include "Engine/Debug.hpp"

#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>
#include <bullet/BulletCollision/CollisionShapes/btCollisionShape.h>
#include <bullet/BulletCollision/CollisionShapes/btTriangleMeshShape.h>

#include <cmath>

namespace cinek {
    namespace ove {
  

const SceneFixedBodyHull* SceneBody::getFixedHull() const
{
    const SceneFixedBodyHull* hull = nullptr;
    if (btBody) {
        const btCollisionShape* shape = this->btBody->getCollisionShape();
        if (shape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE) {
            const btTriangleMeshShape* triMeshShape =
                static_cast<const btTriangleMeshShape*>(shape);
            hull = static_cast<const SceneFixedBodyHull*>(triMeshShape->getMeshInterface());
        }
    }
    return hull;
}

void SceneBody::setPosition
(
    const btVector3 &pos,
    btVector3 up
)
{
    CK_ASSERT(this->btBody->isStaticOrKinematicObject());

    btVector3 forward(0,0,1);
    
    if (up.isZero()) {
        up.setValue(0,1,0);
    }
    
    //  determine forward vector based on our world up direction
    btScalar dirdot = std::abs(forward.dot(up));
    if (dirdot > btScalar(0.90)) {
        if (up.y() >=btScalar(0)) {
            forward = btVector3(0,-1,0);
        }
        else {
            forward = btVector3(0,1,0);
        }
    }

    btVector3 side = btCross(forward, up);
    side.normalize();
    
    forward = btCross(up, side);
    forward.normalize();
    
    btTransform transform;
    transform.getBasis().setValue(side.x(), side.y(), side.z(),
                                  up.x(), up.y(), up.z(),
                                  -forward.x(), -forward.y(), -forward.z());
    transform.setOrigin(pos);
    
    this->btBody->setWorldTransform(transform);
    if (this->motionState) {
        this->motionState->setWorldTransform(transform);
    }
}
  
  
    } /* namespace ove */
} /* namespace cinek */
