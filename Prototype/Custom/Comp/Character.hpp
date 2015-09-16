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

struct CharacterComponent
{
    COMPONENT_DEFINITION(CharacterComponent);

    //  the party this character belongs to (i.e. a player's party, or a fleet)
    Entity partyEntity;
    //  the party that's a 'slave' to this Character (i.e. a ship's crew)
    Entity slavePartyEntity;
};

} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_App_Custom_Comp_Character_hpp) */
