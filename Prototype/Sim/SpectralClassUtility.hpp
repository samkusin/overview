//
//  SpectralClassUtility.hpp
//  Overview
//
//  Created by Samir Sinha on 7/11/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Sim_SpectralClassUtility_hpp
#define Overview_Sim_SpectralClassUtility_hpp

#include "SimDefines.hpp"

#include <cinek/json/json.hpp>

namespace cinek { namespace ovproto {

//  A series of utilities that relate stellar characteristics
//  Many of the values returned by this utility are obtained through a mix of
//  calculated and observed values.   Calculated values are typically
//  approximations - basically there's seldom a single catch-all equation to
//  calculate many stellar characteristics.
//
//  Basically, these functions are (perhaps overly) good enough for a game.
//  We'll point out which functions calculate vs lookup/interpolate the returned
//  value.
//
//  References:
//      Constants:   http://www.astro.princeton.edu/~gk/A403/constants.pdf
//      Calibrations: http://xoomer.virgilio.it/hrtrace/Sk.htm (Schmidt-Kaler)
//                    http://xoomer.virgilio.it/hrtrace/Straizys.htm (Straizys)
//      Colors:  http://www.vendian.org/mncharity/dir3/starcolor/UnstableURLs/starcolors.txt
//      Calculated: http://www.uni.edu/morgans/stars/b_v.html
//
namespace SpectralUtility
{

//  sets up the temp -> rgb table
//  sets up the bolemetric correction table
//  sets up the class.type [O,B,....,M] -> temperature, mass table
bool loadTables();

//  releases tables
void unloadTables();

//  returns kSolBolometricMag - 2.5*log10(solarL)
//
ckm::scalar bolometricMagFromLuminosity(ckm::scalar solarL);

//  Spectral class based on effective temperature (and stage when we have one.)
//
SpectralClass classFromTemp(int tempK);
    
//  lookup based on temperature and Spectral Luminosity Class (I,II,...,V, etc.)
//  using the Schmidt-Kaler Bolometric Correction table (see references)
//
ckm::scalar bolmetricCorrection(int tempK, SpectralClass mkClass);

//  lookup based on temperature
//  using the table constructed by Mitchell Charity (see references)
uint32_t colorABGRFromClass(int tempK);

}

} /* namespace ovproto */ } /* namespace cinek */

#endif
