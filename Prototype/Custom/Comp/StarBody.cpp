//
//  StarBody.cpp
//  Overview
//
//  Created by Samir Sinha on 7/31/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "StarBody.hpp"
#include "Sim/SpectralClassUtility.hpp"

namespace cinek { namespace ovproto {

COMPONENT_TYPE_IMPL(StarBodyComponent, kStarBodyComponent, {});

void StarBodyComponent::init
(
    ckm::scalar mass,
    ckm::scalar radius,
    ckm::scalar luminosity
)
{
    _solarMass = mass;
    _solarRadius = radius;
    _solarLuminosity = luminosity;
    
    _effectiveTemp = SpectralUtility::radiusAndLuminosityToEffTemp(
        _solarRadius,
        _solarLuminosity
    );

    _abgrColor = SpectralUtility::colorABGRFromTemp(_effectiveTemp);
    
    SpectralClass mkClass;
    _visualMag = SpectralUtility::bolometricMagFromLuminosity(_solarLuminosity);
    _visualMag -= SpectralUtility::bolmetricCorrection(_effectiveTemp, mkClass);
}

} /* namespace overview */ } /* namespace cinek */