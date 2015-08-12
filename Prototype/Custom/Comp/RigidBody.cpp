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

namespace component
{
    COMPONENT_TYPE_IMPL(RigidBody, kRigidBody, {

    });
 
    void RigidBody::init
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
        
        ckm::mat3 inertiaTensors(1);
        inertiaTensors[0].x = xI;
        inertiaTensors[1].y = yI;
        inertiaTensors[2].z = zI;
        _invInertiaTensors = ckm::inverse(inertiaTensors);
        
        //  handle cases where we call init with an active rigid body.
        //  since we're changing mass, immediately change fields affected by
        //  mass.
        //  This is likely not a good way to deal with mass changes, but we'll
        //  see (and normally mass shouldn't change during an active sim?)
        _momentum = _mass * _velocity;
        _angularVelocity = _angularMomentum * _invInertiaTensors;
    }
    
    auto RigidBody::integrate
    (
        LocalTransform transform,
        ckm::scalar dt
    )
    -> LocalTransform
    {
        //  calculate values based on the current transform needed for the
        //  below integration - this is necessary since RigidBody can't tell if
        //  the incoming orientation has changed prior to the last call to
        //  integrate.
        //
        _axialSpin =  0.5 *
            ckm::quat(_angularVelocity.x, _angularVelocity.y, _angularVelocity.z, 0) *
            transform.orient;
    
        transform.position += _velocity * dt;
        _momentum += _force * dt;
        transform.orient += _axialSpin * dt;
        _angularMomentum += _torque * dt;
    
        //  calculate new values that may be accessed by an external system
        _velocity = _momentum * _invMass;
        _angularVelocity = _angularMomentum * _invInertiaTensors;
        _axialSpin = 0.5 *
            ckm::quat(_angularVelocity.x, _angularVelocity.y, _angularVelocity.z, 0) *
            transform.orient;
        
        return transform;
    }
}


} /* namespace ovproto */ } /* namespace cinek */