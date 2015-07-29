//
//  StarBody.hpp
//  Overview
//
//  Created by Samir Sinha on 6/30/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Custom_Comp_StarBody_hpp
#define Overview_App_Custom_Comp_StarBody_hpp


#include "CustomComponentTypes.hpp"

namespace cinek { namespace ovproto {

namespace component
{
    struct StarBody
    {
        COMPONENT_DEFINITION(StarBody, void);

        //  basic properties
        ckm::scalar solarMass;
        ckm::scalar solarRadius;
        ckm::scalar solarLuminosity;
        int32_t effectiveTemp;
        
        //  calculated values from basic properties
        uint32_t abgrColor;             // color from effectiveTemp
        ckm::scalar visualMag;          // visual magnitude
    };
}

} /* namespace overview */ } /* namespace cinek */


#endif
