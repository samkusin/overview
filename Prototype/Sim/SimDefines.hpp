//
//  SimDefines.hpp
//  Overview
//
//  Created by Samir Sinha on 6/30/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Sim_Defines_hpp
#define Overview_Sim_Defines_hpp

#include "Engine/EngineTypes.hpp"

namespace cinek { namespace ovproto {

constexpr ckm::scalar kSolarEffTemp = 5780;

struct SpectralClass
{
    int32_t minEffectiveTemp;           // in Kelvin
    ckm::scalar minSolarMass;           // minimum solar mass
    ckm::scalar minSolarRadius;         // minimum solar radius
    ckm::scalar minSolarLuminosity;     // minimum solar output
    ckm::scalar pctChanceHasCompanion;  // chance body will have a companion
    int   companionCurveExp;            // chance body is a companion of another
    enum { kWeibull, kUniform } distFn; // distrivution function of
};

} /* namespace ovproto */ } /* namespace cinek */

#endif
