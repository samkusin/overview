//
//  NavBody.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/16/16.
//
//

#include "NavBody.hpp"

namespace cinek {
    namespace ove {
    
NavBody::NavBody(InitProperties initProperties) :
    _entity(initProperties.entity),
    _transform(nullptr),
    _speedCurve(initProperties.speedCurve)
{
}

NavBody::NavBody
(
    const NavBody& source,
    Entity entity
) :
    _entity(entity),
    _transform(nullptr),
    _speedCurve(source._speedCurve)
{
}

void NavBody::setNavBodyTransform(NavBodyTransform* transform)
{
    _transform = transform;
}
    
    } /* namespace ove */
} /* namespace cinek */