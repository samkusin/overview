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


namespace cinek { namespace ovproto {

namespace component
{
    COMPONENT_TYPE_IMPL(StellarSystem, kStellarSystem, {}, {});
    COMPONENT_TYPE_IMPL(StarBody, kStarBody, {}, {});
}

} /* namespace overview */ } /* namespace cinek */
