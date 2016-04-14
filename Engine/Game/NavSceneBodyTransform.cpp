//
//  NavSceneBodyTransform.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/16/16.
//
//

#include "NavSceneBodyTransform.hpp"
#include "Engine/Physics/SceneTypes.hpp"

namespace cinek {
    namespace ove {
    
NavSceneBodyTransform::NavSceneBodyTransform(SceneBody* body) :
    _sceneBody(body)
{
}
    
void NavSceneBodyTransform::getNavBodyTransform
(
    ckm::quat& basis,
    ckm::vector3& translate
)
{
    _sceneBody->getTransform(basis, translate);
}

void NavSceneBodyTransform::setNavBodyTransform
(
    ckm::quat basis,
    ckm::vector3 translate
)
{
    _sceneBody->setTransform(basis, translate);
}

void NavSceneBodyTransform::setNavBodyVelocity
(
    ckm::vector3 linear,
    ckm::vector3 angular
)
{
    _sceneBody->setVelocity(linear, angular);
}

    } /* namespace ove */
} /* namespace cinek */