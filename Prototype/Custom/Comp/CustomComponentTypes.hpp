//
//  CustomComponentTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 6/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Comp_Custom_ComponentTypes_hpp
#define Overview_App_Comp_Custom_ComponentTypes_hpp

#include "Engine/Entity/EntityTypes.hpp"

namespace cinek { namespace ovproto {


namespace component
{
    struct StellarSystem;
    struct StarBody;
    struct Loadout;
    struct Party;
    struct Character;
    struct RigidBody;
    
    enum
    {
        kStellarSpace       = overview::component::kCustomSpace0,
        kEntityGroupSpace   = overview::component::kCustomSpace0+1,
        kActorSpace         = overview::component::kCustomSpace0+2,
        kPhysicsSpace       = overview::component::kCustomSpace0+3
    };
    
    enum
    {
        kStellarSystem      = overview::component::MakeComponentId(kStellarSpace, 0x0000),
        kStarBody           = overview::component::MakeComponentId(kStellarSpace, 0x0001),
        kLoadout            = overview::component::MakeComponentId(kEntityGroupSpace, 0x0000),
        kParty              = overview::component::MakeComponentId(kEntityGroupSpace, 0x0001),
        kCharacter          = overview::component::MakeComponentId(kActorSpace, 0x0000),
        kRigidBody          = overview::component::MakeComponentId(kPhysicsSpace, 0x0000)
    };
    
    enum
    {
        kLoadout_Player_GroupId         = 0x00000000,
        kLoadout_SmallShip_GroupId      = 0x00000001,
        kParty_Player_GroupId           = 0x00010000,
        kParty_SmallShip_GroupId        = 0x00010001
    };
}

} /* namespace ovproto */ } /* namespace cinek */

#endif
