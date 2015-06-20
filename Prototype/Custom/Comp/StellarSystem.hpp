//
//  StellarSystem.hpp
//  Overview
//
//  Created by Samir Sinha on 6/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Comp_Custom_StellarSystem_hpp
#define Overview_App_Comp_Custom_StellarSystem_hpp

#include "CustomComponentTypes.hpp"

namespace cinek { namespace ovproto {

namespace component
{
    struct StellarSystem
    {
        COMPONENT_DEFINITION(StellarSystem);

        uint32_t seed;
        intptr_t graphData;
    };
}

} /* namespace overview */ } /* namespace cinek */

#endif
