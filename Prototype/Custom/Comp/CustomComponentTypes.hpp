//
//  CustomComponentTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 6/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Comp_Custom_ComponentTypes_hpp
#define Overview_App_Comp_Custom_ComponentTypes_hpp

#include "GameTypes.hpp"
#include "Engine/Entity/EntityTypes.hpp"

namespace cinek { namespace ovproto {

namespace component
{
    enum
    {
        kStellarSpace       = overview::component::kCustomSpace0
    };
    
    enum
    {
        kStellarSystem      = overview::component::MakeComponentId(kStellarSpace, 0x0000),
    };
}

} /* namespace ovproto */ } /* namespace cinek */

#endif