//
//  RigidBodyPhysics.cpp
//  Overview
//
//  Created by Samir Sinha on 8/10/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "RigidBody.hpp"

#include <cinek/debug.hpp>

namespace cinek { namespace ovproto {

COMPONENT_TYPE_IMPL(RigidBodyComponent, kRigidBodyComponent, {

});

void RigidBodyComponent::init
(
    ckm::scalar mass,
    ckm::scalar xI,
    ckm::scalar yI,
    ckm::scalar zI
)
{
    if (_mass != mass)
    {
        _mass = mass;
        _invMass = 1/mass;
    }
    
    _inertiaTensors = ckm::mat3(0);
    _inertiaTensors[0].x = xI;
    _inertiaTensors[1].y = yI;
    _inertiaTensors[2].z = zI;
    _invInertiaTensors = ckm::inverse(_inertiaTensors);
    
    //  handle cases where we call init with an active rigid body.
    //  since we're changing mass, immediately change fields affected by
    //  mass.
    //  This is likely not a good way to deal with mass changes, but we'll
    //  see (and normally mass shouldn't change during an active sim?)
    _momentum = _mass * _velocity;
    _angularVelocity = _angularMomentum * _invInertiaTensors;
}

auto RigidBodyComponent::integrate
(
    LocalTransform transform,
    ckm::scalar dt,
    const RigidBodyConstraints& constraints
)
-> LocalTransform
{
    //  calculate values based on the current transform needed for the
    //  below integration - this is necessary since RigidBody can't tell if
    //  the incoming orientation has changed prior to the last call to
    //  integrate.
    //
    _axialSpin =  0.5 *
        ckm::quat(0, _angularVelocity.x, _angularVelocity.y, _angularVelocity.z) *
        transform.orient;

    transform.position += _velocity * dt;
    _momentum += _force * dt;
    transform.orient = ckm::normalize(transform.orient + _axialSpin * dt);
    
    _angularMomentum += _torque * dt;

    //  calculate new values that may be accessed by an external system
    _velocity = _momentum * _invMass;
    if (ckm::vectorLength(_velocity) > constraints.maxLinearSpeed)
    {
        _velocity = ckm::normalize(_velocity);
        _velocity *= constraints.maxLinearSpeed;
    }
    
    _angularVelocity = _angularMomentum * _invInertiaTensors;
    
    _axialSpin = 0.5 *
        ckm::quat(0, _angularVelocity.x, _angularVelocity.y, _angularVelocity.z) *
        transform.orient;
    
    
    return transform;
}

void RigidBodyComponent::stop()
{
    _angularMomentum = ckm::zero<ckm::vec3>();
    _momentum = ckm::zero<ckm::vec3>();
}

} /* namespace ovproto */ } /* namespace cinek */