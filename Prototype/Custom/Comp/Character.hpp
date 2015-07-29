//
//  Character.hpp
//  Overview
//
//  Created by Samir Sinha on 7/28/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Custom_Comp_Character_hpp
#define Overview_App_Custom_Comp_Character_hpp

#include "CustomComponentTypes.hpp"

namespace cinek { namespace ovproto {

namespace component
{
    struct Character
    {
        COMPONENT_DEFINITION(Character, void);

        //  the party this character belongs to (i.e. a player's party, or a fleet)
        overview::Entity partyEntity;
        //  the party that's a 'slave' to this Character (i.e. a ship's crew)
        overview::Entity slavePartyEntity;
    };
}

} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_App_Custom_Comp_Character_hpp) */
