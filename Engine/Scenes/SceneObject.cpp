//
//  SceneObject.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/4/15.
//
//

#include "SceneObject.hpp"
#include "SceneFixedBodyHull.hpp"

namespace cinek {
    namespace ove {

SceneBody::SceneBody
(
    const btRigidBody::btRigidBodyConstructionInfo& info,
    Entity entity
) :
    SceneObject<btRigidBody>(),
    _entity(entity)
{
    auto obj = ::new(buffer()) btRigidBody(info);
    obj->setUserPointer(this);
    setInitialized(true);
}

    } /* namespace ove */
} /* namespace cinek */
