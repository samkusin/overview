//
//  CustomComponents.cpp
//  Overview
//
//  Created by Samir Sinha on 6/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "CustomComponentTypes.hpp"
#include "StellarSystem.hpp"
#include "StarBody.hpp"
#include "Loadout.hpp"
#include "Party.hpp"

#include "Engine/Entity/EntityStore.hpp"

namespace cinek { namespace ovproto {

namespace component
{
    COMPONENT_TYPE_IMPL(StellarSystem, kStellarSystem, {});
    COMPONENT_TYPE_IMPL(StarBody, kStarBody, {});
    COMPONENT_TYPE_IMPL(Loadout, kLoadout, {});
    COMPONENT_TYPE_IMPL(Party, kParty, {});
}

} /* namespace overview */ } /* namespace cinek */
