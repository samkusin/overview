//
//  CustomComponents.cpp
//  Overview
//
//  Created by Samir Sinha on 6/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "CustomComponentTypes.hpp"
#include "Loadout.hpp"
#include "Party.hpp"

namespace cinek { namespace ovproto {

COMPONENT_TYPE_IMPL(LoadoutComponent, kLoadoutComponent, {});
COMPONENT_TYPE_IMPL(PartyComponent, kPartyComponent, {});

} /* namespace overview */ } /* namespace cinek */
