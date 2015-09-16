//
//  RigidBodySystem.hpp
//  Overview
//
//  Created by Samir Sinha on 8/11/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Physics_RigidBodySystem_hpp
#define Overview_Physics_RigidBodySystem_hpp

#include "Custom/Comp/RigidBody.hpp"
#include "Engine/Entity/Comp/Transform.hpp"

#include <cinek/entity/entitydatatable.hpp>

namespace cinek { namespace ovproto {

//  Simulates 
void simulateRigidBodies
(
    component::Table<RigidBodyComponent> rigidBodies,
    component::Table<overview::TransformComponent> transforms,
    double dt,
    const RigidBodyConstraints& constraints
);

} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_Physics_RigidBodySystem_hpp) */
