//
//  SimDefines.hpp
//  Overview
//
//  Created by Samir Sinha on 6/30/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Sim_Defines_hpp
#define Overview_Sim_Defines_hpp

#include "GameTypes.hpp"

namespace cinek { namespace ovproto {

//  Simulation Settings
//
constexpr ckm::scalar kMaxStarPairDistInSolRadius = 10;
constexpr ckm::scalar kSystemInnerRadiusFraction = 0.33;
constexpr int kMaxStarsPerSystem = 4;
constexpr int kMaxStarSystems = 512*1024;
constexpr int kMaxStars = kMaxStarSystems * kMaxStarsPerSystem;
constexpr int kMaxRigidBodies = 128;
constexpr int kMaxTransforms = kMaxStarSystems + kMaxStars + kMaxRigidBodies;
constexpr int kMaxRenderables = kMaxStarSystems + kMaxStars + kMaxRigidBodies;
constexpr int kMaxEntities = kMaxTransforms * 2;

//  References:
//      Basic Distance Constants: Wikipedia
//      Constants:   http://www.astro.princeton.edu/~gk/A403/constants.pdf
//
constexpr ckm::scalar kSolEffTemp = 5780;
constexpr ckm::scalar kKmPerLYR = 9.4605284e12;
constexpr ckm::scalar kKmPerAU = 149597871;
constexpr ckm::scalar kAUPerLYR = 63239.726183647;
constexpr ckm::scalar kKmPerSolRadius = 695800;
constexpr ckm::scalar kSolRadiusPerAU = 215.00125180;
constexpr ckm::scalar kSolBolometricMag = 4.8;

struct SpectralClassDesc
{
    int32_t minEffectiveTemp;           // in Kelvin
    ckm::scalar minInitialSolarMass;    // minimum solar mass
    ckm::scalar pctChanceHasCompanion;  // chance body will have a companion
    int   companionCurveExp;            // chance body is a companion of another
    enum { kWeibull, kUniform } distFn; // distribution of companion star class
};

//  MK Class Parser - used as a key when looking up colors, and as 'real-world'
//  information describing a star's class.
struct SpectralClass
{
    enum Type { Degenerate, O, B, A, F, G, K, M, kTypeCount };
    enum Luminosity {Undefined, Ia, Iab, Ib, II, III, IV, V, sd, D, kSubclassCount };

    SpectralClass() : key(0) {}
    SpectralClass(uint32_t k) : key(k) {}
    SpectralClass(Type t, uint8_t sc, Luminosity l);
    
    Type type() const;
    Luminosity luminosity() const;
    uint8_t subclass() const;

    operator uint32_t() const { return key; }

    uint32_t key;
};

inline SpectralClass::SpectralClass(Type t, uint8_t sc, Luminosity l) :
    key( (((uint32_t)t) << 24) |
         (((uint32_t)sc) << 16) |
          ((uint32_t)l) )
{
}

inline SpectralClass::Type SpectralClass::type() const {
    return (SpectralClass::Type)(key >> 24);
}

inline uint8_t SpectralClass::subclass() const {
    return (uint8_t)((key >> 16) & 0xff);
}

inline SpectralClass::Luminosity SpectralClass::luminosity() const {
    return (SpectralClass::Luminosity)(key & 0xff);
}

} /* namespace ovproto */ } /* namespace cinek */

#endif
