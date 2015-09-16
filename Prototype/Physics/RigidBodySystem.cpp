//
//  RigidBodySystem.cpp
//  Overview
//
//  Created by Samir Sinha on 8/11/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "RigidBodySystem.hpp"

namespace cinek { namespace ovproto {

struct SimulateRigidBody
{
    component::Table<overview::TransformComponent> transforms;
    RigidBodyConstraints constraints;
    
    double dt;
    
    void operator()(Entity e, RigidBodyComponent& body)
    {
        auto transform = transforms.dataForEntity(e);
        if (transform)
        {
            RigidBodyComponent::LocalTransform lt = {
                transform->localPosition(), transform->localOrient()
            };
            lt = body.integrate(lt, dt, constraints);
            transform->setLocalPosition(lt.position);
            transform->setLocalOrient(lt.orient);
        }
    }
};

//  Simulates 
void simulateRigidBodies
(
    component::Table<RigidBodyComponent> rigidBodies,
    component::Table<overview::TransformComponent> transforms,
    double dt,
    const RigidBodyConstraints& constraints
)
{
    SimulateRigidBody simBody;
    simBody.transforms = transforms;
    simBody.dt = dt;
    simBody.constraints = constraints;
    
    rigidBodies.forEach(simBody);
}

} /* namespace ovproto */ } /* namespace cinek */
