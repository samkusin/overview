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

void SceneBody::activate()
{
    if ((savedState.flags & SavedState::kDynamic)!=0) {
        btBody->setMassProps(savedState.mass, savedState.inertia);
    }
    
    savedState.flags = SavedState::kNone;
}

void SceneBody::deactivate()
{
    if (!btBody->isStaticOrKinematicObject()) {
        savedState.flags |= SavedState::kDynamic;
        
        savedState.mass = 1/btBody->getInvMass();
        const btVector3& invInertia = btBody->getInvInertiaDiagLocal();
        savedState.inertia.setValue(
            invInertia.x() != btScalar(0) ? 1/invInertia.x() : btScalar(0),
            invInertia.y() != btScalar(0) ? 1/invInertia.y() : btScalar(0),
            invInertia.z() != btScalar(0) ? 1/invInertia.z() : btScalar(0)
        );
        
        btBody->setMassProps(btScalar(0), btVector3(0,0,0));
    }
}
    
    } /* namespace ove */
} /* namespace cinek */
