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
    _transform->getNavBodyTransform(_rotation, _position);
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

auto NavBody::updatePath(dtPolyRef currentPoly) -> State
{
    if (_state == State::kPathRun) {
        if (!_path) {
            _state = State::kPathEnd;
        }
        else if (!_path.updatePath(currentPoly)) {
            _state = State::kPathBreak;
        }
    }
    return _state;
}

void NavBody::setToIdle()
{
    _state = State::kIdle;
    _path.clear();
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
    ckm::quat orient,
    ckm::vector3 position
)
{
    _transform->setNavBodyTransform(std::move(orient), std::move(position));
}

void NavBody::pushTransformVelocity
(
    ckm::vector3 linear,
    ckm::vector3 angular
)
{
    _transform->setNavBodyVelocity(std::move(linear), std::move(angular));
}
    
    } /* namespace ove */
} /* namespace cinek */