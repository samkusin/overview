//
//  SceneComponent.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 4/21/16.
//
//

#include "Engine/Component.hpp"
#include "SceneTypes.hpp"

namespace cinek {
    namespace ove {
    
template<> const char* Component<SceneBody>::sPropertyNames[] = {
    SceneBody::kPositionLabel,
    SceneBody::kRotationLabel,
    SceneBody::kVelocityLabel,
    SceneBody::kAngularVelocityLabel
};

template<> const int Component<SceneBody>::sPropertyCount = sizeof(sPropertyNames)/sizeof(sPropertyNames[0]);

template<> Entity Component<SceneBody>::getEntity() const
{
    return _object->entity;
}

template<> bool Component<SceneBody>::readProperty
(
    const char* name,
    ComponentData& data
)
const
{
    if (!strncmp(name, SceneBody::kPositionLabel, sizeof( SceneBody::kPositionLabel))) {
        data.set(_object->getPosition());
        return true;
    }
    else if (!strncmp(name,  SceneBody::kRotationLabel, sizeof( SceneBody::kRotationLabel))) {
        data.set(_object->getRotation());
        return true;
    }
    else if (!strncmp(name, SceneBody::kVelocityLabel, sizeof( SceneBody::kVelocityLabel))) {
        data.set(_object->getVelocity());
        return true;
    }
    else if (!strncmp(name, SceneBody::kAngularVelocityLabel, sizeof( SceneBody::kAngularVelocityLabel))) {
        data.set(_object->getAngularVelocity());
        return true;
    }
    return false;
}

template<> bool Component<SceneBody>::writeProperty
(
    const char* name,
    const ComponentData& data
)
{
    if (!strncmp(name, SceneBody::kPositionLabel, sizeof(SceneBody::kPositionLabel))) {
        _object->setPosition(data.get<ckm::vector3>());
        return true;
    }
    else if (!strncmp(name, SceneBody::kRotationLabel, sizeof(SceneBody::kRotationLabel))) {
        _object->setRotation(data.get<ckm::quat>());
        return true;
    }
    else if (!strncmp(name, SceneBody::kVelocityLabel, sizeof( SceneBody::kVelocityLabel))) {
        _object->setVelocity(data.get<ckm::vector3>());
        return true;
    }
    else if (!strncmp(name, SceneBody::kAngularVelocityLabel, sizeof( SceneBody::kAngularVelocityLabel))) {
        _object->setAngularVelocity(data.get<ckm::vector3>());
        return true;
    }
    return false;
}


template class Component<SceneBody>;


    } /* namespace ove */
} /* namespace cinek */
