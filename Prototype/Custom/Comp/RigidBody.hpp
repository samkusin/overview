//
//  RigidBodyPhysics.hpp
//  Overview
//
//  Created by Samir Sinha on 8/10/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Custom_Comp_RigidBodyPhysics
#define Overview_App_Custom_Comp_RigidBodyPhysics

#include "CustomComponentTypes.hpp"
#include "Engine/EngineMath.hpp"

//  design references: http://gafferongames.com/game-physics/physics-in-3d/

namespace cinek { namespace ovproto {

namespace component
{

    struct RigidBodyConstraints
    {
        ckm::scalar maxLinearSpeed;
    };

    struct RigidBody
    {
        COMPONENT_DEFINITION(RigidBody);

        //  setup attributes.
        void init(ckm::scalar mass, ckm::scalar xI, ckm::scalar yI, ckm::scalar zI);
        
        //  attribute access
        ckm::scalar mass() const { return _mass; }
        const ckm::vec3& velocity() const { return _velocity; }
        const ckm::vec3& angularVelocity() const { return _angularVelocity; }
        
        const ckm::vec3& force() const { return _force; }
        const ckm::vec3& torque() const { return _torque; }
        const ckm::mat3& inertiaTensors() const { return _inertiaTensors; }
        
        void setForce(const ckm::vec3& force) { _force = force; }
        void setTorque(const ckm::vec3& torque) { _torque = torque; }
                
        //  updates the body using the supplied forces.
        struct LocalTransform
        {
            ckm::vec3 position;
            ckm::quat orient;
        };
        
        LocalTransform integrate(LocalTransform transform, ckm::scalar dt,
                                 const RigidBodyConstraints& constraints);
        
    private:
        ckm::vec3 _momentum;
        ckm::vec3 _angularMomentum;

        // calculated
        ckm::vec3 _velocity;
        ckm::vec3 _angularVelocity;
        ckm::quat _axialSpin;
        
        // attributes
        ckm::scalar _mass;
        ckm::scalar _invMass;
        ckm::mat3 _inertiaTensors;
        ckm::mat3 _invInertiaTensors;
        
        ckm::vec3 _force;
        ckm::vec3 _torque;
    };
}

} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_App_Custom_Comp_RigidBodyPhysics) */
