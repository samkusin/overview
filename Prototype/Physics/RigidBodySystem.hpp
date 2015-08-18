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
#include "Engine/Entity/Transform.hpp"
#include "Engine/Entity/EntityDataTable.hpp"

namespace cinek { namespace ovproto {

//  Simulates 
void simulateRigidBodies
(
    overview::component::Table<component::RigidBody> rigidBodies,
    overview::component::Table<overview::component::Transform> transforms,
    double dt
);

} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_Physics_RigidBodySystem_hpp) */
