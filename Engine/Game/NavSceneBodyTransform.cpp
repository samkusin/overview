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
    ckm::vector3f& translate,
    ckm::matrix3f& basis
)
{
    _sceneBody->getTransform(basis, translate);
}

void NavSceneBodyTransform::setNavBodyTransform
(
    ckm::vector3f translate,
    ckm::matrix3f basis
)
{
    _sceneBody->setTransform(basis, translate);
}

    } /* namespace ove */
} /* namespace cinek */