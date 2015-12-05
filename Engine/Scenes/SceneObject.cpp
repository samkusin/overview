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

SceneTriangleMeshShape::SceneTriangleMeshShape
(
    SceneFixedBodyHull* hull,
    std::string name
) :
    SceneObject<btBvhTriangleMeshShape>(),
    _name(std::move(name))
{
    auto obj = ::new(buffer()) btBvhTriangleMeshShape(hull, false);
    obj->setUserPointer(this);
}
    



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
}

    } /* namespace ove */
} /* namespace cinek */
