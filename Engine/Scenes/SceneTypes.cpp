//
//  SceneTypes.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/6/16.
//
//

#include "SceneTypes.hpp"

#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>

namespace cinek {
    namespace ove {

void SceneBodyMassProps::fromRigidBody(const btRigidBody& btBody)
{
    mass = 1/btBody.getInvMass();
    const btVector3& invInertia = btBody.getInvInertiaDiagLocal();
    inertia.setValue(
        invInertia.x() != btScalar(0) ? 1/invInertia.x() : btScalar(0),
        invInertia.y() != btScalar(0) ? 1/invInertia.y() : btScalar(0),
        invInertia.z() != btScalar(0) ? 1/invInertia.z() : btScalar(0)
    );
}
  
    } /* namespace ove */
} /* namespace cinek */
