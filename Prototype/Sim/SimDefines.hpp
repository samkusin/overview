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

constexpr ckm::scalar kSolEffTemp = 5780;
constexpr ckm::scalar kKmPerLYR = 9.4605284e12;
constexpr ckm::scalar kKmPerAU = 149597871;
constexpr ckm::scalar kAUPerLYR = 63239.726183647;
constexpr ckm::scalar kKmPerSolRadius = 695800;
constexpr ckm::scalar kSolRadiusPerAU = 215.00125180;
constexpr ckm::scalar kMaxStarPairDistInSolRadius = 10;
constexpr ckm::scalar kSystemInnerRadiusFraction = 0.33;

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
