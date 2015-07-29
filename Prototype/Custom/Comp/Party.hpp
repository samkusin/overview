//
//  Party.hpp
//  Overview
//
//  Created by Samir Sinha on 7/28/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Custom_Comp_Party_hpp
#define Overview_App_Custom_Comp_Party_hpp

#include "CustomComponentTypes.hpp"
#include "Engine/Entity/EntityGroup.hpp"

namespace cinek { namespace ovproto {

namespace component
{
    struct Party
    {
        COMPONENT_DEFINITION(Party, overview::EntityStore);

        enum class Type
        {
            kNone,
            kPlayer,
            kSmallShip
        };
        
        static constexpr overview::EntityGroupMapId kGroupId_Player = kParty_Player_GroupId;
        static constexpr overview::EntityGroupMapId kGroupId_SmallShip = kParty_SmallShip_GroupId;
        
        Type type;
    };
}

} /* namespace ovproto */ } /* namespace cinek */


#endif
