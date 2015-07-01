//
//  StellarSystem.hpp
//  Overview
//
//  Created by Samir Sinha on 6/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Custom_Comp_StellarSystem_hpp
#define Overview_App_Custom_Comp_StellarSystem_hpp

#include "CustomComponentTypes.hpp"

namespace cinek { namespace ovproto {

namespace component
{
    struct StellarSystem
    {
        COMPONENT_DEFINITION(StellarSystem);

        //  seed used to generate planets for this system
        uint32_t seed;
        //  filled in by the StellarSystemUtility
        intptr_t indexToTreeNode;
        //  radius of the system
        ckm::scalar radius;
    };
}

} /* namespace overview */ } /* namespace cinek */

#endif
