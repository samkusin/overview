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

struct StellarSystemComponent;
struct StarBodyComponent;
struct LoadoutComponent;
struct PartyComponent;
struct CharacterComponent;
struct RigidBodyComponent;
    
enum
{
    kStellarComponentSpace       = overview::kCustomComponentSpace0,
    kEntityGroupComponentSpace   = overview::kCustomComponentSpace0+1,
    kActorComponentSpace         = overview::kCustomComponentSpace0+2,
    kAIPhysicsComponentSpace     = overview::kCustomComponentSpace0+3
};

enum
{
    kStellarSystemComponent      = MakeComponentId(kStellarComponentSpace, 0x0000),
    kStarBodyComponent           = MakeComponentId(kStellarComponentSpace, 0x0001),
    kLoadoutComponent            = MakeComponentId(kEntityGroupComponentSpace, 0x0000),
    kPartyComponent              = MakeComponentId(kEntityGroupComponentSpace, 0x0001),
    kCharacterComponent          = MakeComponentId(kActorComponentSpace, 0x0000),
    kRigidBodyComponent          = MakeComponentId(kAIPhysicsComponentSpace, 0x0000),
    kSteeringComponent           = MakeComponentId(kAIPhysicsComponentSpace, 0x0001)
};

enum
{
    kLoadout_Player_GroupId         = 0x00000000,
    kLoadout_SmallShip_GroupId      = 0x00000001,
    kParty_Player_GroupId           = 0x00010000,
    kParty_SmallShip_GroupId        = 0x00010001
};

} /* namespace ovproto */ } /* namespace cinek */

#endif
