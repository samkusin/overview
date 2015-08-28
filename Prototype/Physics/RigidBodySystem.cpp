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
    overview::component::Table<overview::component::Transform> transforms;
    component::RigidBodyConstraints constraints;
    
    double dt;
    
    void operator()(overview::Entity e, component::RigidBody& body)
    {
        auto transform = transforms.dataForEntity(e);
        if (transform)
        {
            component::RigidBody::LocalTransform lt = {
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
    overview::component::Table<component::RigidBody> rigidBodies,
    overview::component::Table<overview::component::Transform> transforms,
    double dt,
    const component::RigidBodyConstraints& constraints
)
{
    SimulateRigidBody simBody;
    simBody.transforms = transforms;
    simBody.dt = dt;
    simBody.constraints = constraints;
    
    rigidBodies.forEach(simBody);
}

} /* namespace ovproto */ } /* namespace cinek */
