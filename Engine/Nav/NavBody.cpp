//
//  NavBody.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/16/16.
//
//

#include "NavBody.hpp"
#include "NavBodyTransform.hpp"

namespace cinek {
    namespace ove {
    
NavBody::NavBody(InitProperties initProperties) :
    _entity(initProperties.entity),
    _transform(nullptr),
    _state(State::kIdle)
{
    _speedLimit = initProperties.speedLimit;
    _speed = ckm::scalar(0);
}

NavBody::NavBody
(
    const NavBody& source,
    Entity entity
) :
    _entity(entity),
    _transform(nullptr),
    _state(State::kIdle)
{
    //  cloned properties
    _speedLimit = source._speedLimit;
    
    //  unique properties
    _speed = ckm::scalar(0);
}

void NavBody::setTransform(NavBodyTransform* transform)
{
    _transform = transform;
}

void NavBody::pullTransform()
{
    _transform->getNavBodyTransform(_position, _basis);
}

void NavBody::setPath(NavPath&& path)
{
    _path = std::move(path);
    _state = State::kPathStart;
}

void NavBody::runPath()
{
    if (_path) {
        _state = State::kPathRun;
    }
    else {
        _state = State::kIdle;
    }
}

void NavBody::setSpeedScalar(ckm::scalar speed)
{
    if (speed < 0)
        _speed = 0;
    else if (speed > ckm::scalar(1))
        _speed = ckm::scalar(1);
    else
        _speed = speed;
}
    
ckm::scalar NavBody::calcAbsoluteSpeed() const
{
    return _speed * _speedLimit;
}
    
void NavBody::pushTransformPosOrient
(
    ckm::vector3f position,
    ckm::matrix3f orient
)
{
    _transform->setNavBodyTransform(std::move(position), std::move(orient));
}

void NavBody::pushTransformVelocity
(
    ckm::vector3f linear,
    ckm::vector3f angular
)
{
    _transform->setNavBodyVelocity(std::move(linear), std::move(angular));
}
    
    } /* namespace ove */
} /* namespace cinek */