//
//  EntityRoles.hpp
//  Overview
//
//  Created by Samir Sinha on 7/28/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Sim_EntityRoles_hpp
#define Overview_Sim_EntityRoles_hpp

namespace cinek { namespace ovproto {

//  Party Roles
//
//      Player
enum
{
    kPartyRole_Players
};

//      Ship
enum
{
    kPartyRole_Ship_Captain,
    kPartyRole_Ship_Pilot,
    kPartyRole_Ship_Navigation,
    kPartyRole_Ship_Engineer,
    kPartyRole_Ship_Science,
    kPartyRole_Ship_Medicine
};

//  Loadout Roles
//
//      Player
enum
{
    kLoadoutRole_Players_Hands,
    kLoadoutRole_Players_Body
};

enum
{
    kLoadout_Players_Slot_Hand_Primary,
    kLoadout_Players_Slot_Hand_Secondary,
    kLoadout_Players_Slot_Hand_Count
};

enum
{
    kLoadout_Players_Slot_Body_Suit,
    kLoadout_Players_Slot_Body_Count
};

//      Ship
enum
{
    kLoadoutRole_Ships_Weapons,
    kLoadoutRole_Ships_Missiles
};


} /* namespace ovproto */ } /* namespace cinek */


#endif
