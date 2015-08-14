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
    double dt;
    
    void operator()(overview::Entity e, component::RigidBody& body)
    {
        auto transform = transforms.dataForEntity(e);
        if (transform)
        {
            component::RigidBody::LocalTransform lt = {
                transform->localPosition(), transform->localOrient()
            };
            lt = body.integrate(lt, dt);
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
    double dt
)
{
    SimulateRigidBody simBody;
    simBody.transforms = transforms;
    simBody.dt = dt;
    
    rigidBodies.forEach(simBody);
}

} /* namespace ovproto */ } /* namespace cinek */
