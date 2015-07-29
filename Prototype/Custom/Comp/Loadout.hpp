//
//  Loadout.hpp
//  Overview
//
//  Created by Samir Sinha on 7/28/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Custom_Comp_Loadout_hpp
#define Overview_App_Custom_Comp_Loadout_hpp

#include "CustomComponentTypes.hpp"

namespace cinek { namespace ovproto {

namespace component
{
    struct Loadout
    {
        COMPONENT_DEFINITION(Loadout, overview::EntityStore);

        enum class Type
        {
            kNone,
            kPlayer,
            kSmallShip
        };
        
        static constexpr overview::EntityGroupMapId kGroupId_Player = kLoadout_Player_GroupId;
        static constexpr overview::EntityGroupMapId kGroupId_SmallShip = kLoadout_SmallShip_GroupId;
        
        Type type;
    };
}

} /* namespace ovproto */ } /* namespace cinek */


#endif
