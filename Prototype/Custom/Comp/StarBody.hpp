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
        COMPONENT_DEFINITION(StarBody);

        //  all units are in solar equivalents (i.e. solarMass, solarRadius, etc.)
        //  this method uses the SpectralUtility settings to determine color and
        //  magnitude
        void init(ckm::scalar mass, ckm::scalar radius, ckm::scalar luminosity);
        
        ckm::scalar solarMass() const { return _solarMass; }
        ckm::scalar solarRadius() const { return _solarRadius; }
        ckm::scalar solarLuminosity() const { return _solarLuminosity; }
        
        int32_t effectiveTemp() const { return _effectiveTemp; }
        uint32_t abgrColor() const { return _abgrColor; }
        ckm::scalar visualMagnitude() const { return _visualMag; }
        
    private:
        //  basic properties
        ckm::scalar _solarMass;
        ckm::scalar _solarRadius;
        ckm::scalar _solarLuminosity;
        
        //  calculated values from basic properties
        int32_t _effectiveTemp;         // from radius and luminosity
        uint32_t _abgrColor;            // color from effectiveTemp
        ckm::scalar _visualMag;         // visual magnitude
    };
}

} /* namespace overview */ } /* namespace cinek */


#endif
