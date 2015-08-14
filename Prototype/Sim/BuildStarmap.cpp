//
//  BuildStarmap.cpp
//  Overview
//
//  Created by Samir Sinha on 6/29/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "BuildStarmap.hpp"
#include "SpectralClassUtility.hpp"

#include "Engine/Entity/EntityStore.hpp"
#include "Engine/Entity/Comp/Transform.hpp"
#include "Custom/Comp/StarBody.hpp"
#include "Custom/Comp/StellarSystem.hpp"

#include <cinek/debug.hpp>
#include <cinek/vector.hpp>

#include <random>

namespace cinek { namespace ovproto {

struct BuildStarmapFunction::Randomizer
{
    std::minstd_rand rand;
    std::weibull_distribution<ckm::scalar> weibullDist;
    std::uniform_int_distribution<> intDist;
    std::uniform_real_distribution<ckm::scalar> realDist;
    std::uniform_int_distribution<uint16_t> uint16Dist;
        
    Randomizer() = default;
    Randomizer(uint32_t seed) : rand(seed) {}
    
    decltype(weibullDist)::result_type weibull() {
        return weibullDist(rand);
    }
    decltype(intDist)::result_type uniformInt() {
        return intDist(rand);
    }
    decltype(realDist)::result_type uniformReal() {
        return realDist(rand);
    }
    ckm::scalar uniformPct() {
        auto v = uint16Dist(rand) / (ckm::scalar)(uint16Dist.max() - uint16Dist.min());
        return v * 100;
    }
};

/*
struct Body
{
    enum Stage
    {
        kProtostar,
        kMainSequence,
        kLate,
        kCollapsing,
        kFinal
    };
    Stage stage;                    // stage of the star
    ckm::vec3 offset;               // offset from system center position
    int32_t effectiveTemp;          // i.e. 5600K, etc
    ckm::scalar solarRadius;        // Radius in Sol units
    ckm::scalar solarMass;          // Radius in Sol mass
    ckm::scalar luminosity;         // Bolometric luminosity
    int32_t systemIndex;            // what system this belongs to (if any)
    int32_t siblingBodyIndex;       // body shares the system with this
};
*/

struct BuildStarmapFunction::CommonState
{
    const vector<SpectralClassDesc>& spectralClasses;
    vector<SpectralInput>& spectralInputs;
    int spectralIndexMax;
    vector<Randomizer>& spectralRandomizers;
    simtime ageInYears = 0;
    int starCount;
    
    overview::EntityStore& entityStore;
    StellarSystemTree& stellarSystemTree;
    StandardTables& standardTables;
    
    CommonState(const vector<SpectralClassDesc>& specClasses,
        vector<SpectralInput>& specInputs,
        int specIndexMax,
        vector<Randomizer>& specRandomizers,
        overview::EntityStore& store,
        StellarSystemTree& world,
        StandardTables& tables
    ) :
        spectralClasses(specClasses),
        spectralInputs(specInputs),
        spectralIndexMax(specIndexMax),
        spectralRandomizers(specRandomizers),
        entityStore(store),
        stellarSystemTree(world),
        standardTables(tables),
        starCount(0)
    {
    }
};

    
auto BuildStarmapFunction::operator()
(
    uint32_t masterSeed,
    const vector<SpectralClassDesc>& spectralClasses,
    vector<SpectralInput> spectralInputs,
    int spectralIndexMax,
    ckm::AABB<ckm::vec3> bounds,
    ckm::scalar minSystemRadius,
    ckm::scalar maxSystemRadius
)
-> Result
{
    Result result;
    result.result = Result::kPending;
    
    //  terminate if inputs are incorrect or we're out of memory
    if (spectralClasses.size() != spectralInputs.size())
    {
        result.result = Result::kInvalidInput;
        return result;
    }
    
    std::minstd_rand masterRand(masterSeed);
    std::uniform_int_distribution<uint32_t> seederDist(1,5);
    std::array<uint32_t, 3> seeders;
    for (auto& s : seeders)
    {
        s = seederDist(masterRand);
    }
    
    //  We have separate randomizers per spectral class - because we
    //      generate cells with systems of spectralClass 0 to spectralLevel
    //      where spectralLevel acts as a "LOD" for the cell.
    //      To ensure that we generate the same systems with the same
    //      bodies per master seed, we need to also ensure that each
    //      spectral class has its own seed.
    //
    vector<uint32_t> spectralSeeds(spectralClasses.size());
    std::seed_seq seeder(seeders.begin(), seeders.end());
    seeder.generate(spectralSeeds.begin(), spectralSeeds.end());
    
    vector<Randomizer> spectralRandomizers(spectralSeeds.size());
    for (int i = 0; i < spectralSeeds.size(); ++i)
    {
        spectralRandomizers[i] = Randomizer(spectralSeeds[i]);
    }
    
    ////////////////////////////////////////////////////////////////////////
    overview::EntityStore entityStore(kMaxEntities, {
        { overview::component::Transform::kComponentType, kMaxTransforms },
        { ovproto::component::StellarSystem::kComponentType, kMaxStarSystems },
        { ovproto::component::StarBody::kComponentType, kMaxStars }
    }, {
    
    });
    
    StandardTables standardTables;
    standardTables.transform = entityStore.table<overview::component::Transform>();
    standardTables.systemTable = entityStore.table<ovproto::component::StellarSystem>();
    standardTables.starTable = entityStore.table<ovproto::component::StarBody>();
    
    StellarSystemUtility systemUtility(entityStore);
    
    StellarSystemTree stellarSystemTree(entityStore.capacity()*8, systemUtility);
    
    CommonState common(spectralClasses, spectralInputs, spectralIndexMax,
        spectralRandomizers,
        entityStore,
        stellarSystemTree,
        standardTables
    );
    
    //  generate stellar bodies for every configured spectral class
    //  every class has its own seed used for body generation
    //
    int classIndex = 0;
    
    for (auto& spectralInput : spectralInputs)
    {
        if (result.result != Result::kPending)
    break;
        
        if (classIndex > spectralIndexMax)
    continue;
     
        //  spectral class 0 is 'max possible star class + 1' reference so
        //  we can have min/max ranges for various attributes.
        if (classIndex > 0)
        {
            auto& spectralClass = common.spectralClasses[classIndex];
            
            while (spectralInput.solarMassPool >= spectralClass.minInitialSolarMass &&
                   result.result == Result::kPending)
            {
                //  Determine base system radius - which relies on the amount
                //  of mass in the system.
                //
                //  This mass is assumed to be the combined mass of all stars
                //  in the system (where ~ 98%+ of a system's mass typically
                //  belongs to stars vs planets, asteroids, comets, etc.)
                //
                //  Mass ~ Mass of Body 1 + Body 2 + ... + Body N
                //
                std::array<StarTemplate, kMaxBodiesPerSystem> bodyTemplates;
                int bodyClassIndex = classIndex;
                ckm::scalar pctChanceCompanionScalar = 1.0;
                ckm::scalar adjMinSystemRadius = minSystemRadius;
                for (auto& bodyTemplate : bodyTemplates)
                {
                    bodyTemplate.classIndex = bodyClassIndex;
                    if (bodyClassIndex < 0)
                break;
                
                    auto& prevSpectral = common.spectralClasses[bodyClassIndex-1];
                    auto& thisSpectral = common.spectralClasses[bodyClassIndex];
                    auto& randomizer = common.spectralRandomizers[bodyClassIndex];
                    auto& thisInput = common.spectralInputs[bodyClassIndex];
                    
                    bodyTemplate.mass = calcBodyMass(prevSpectral.minInitialSolarMass,
                        thisSpectral.minInitialSolarMass,
                        randomizer
                    );
                    //  on first iteration, the initial solarMassPool will be
                    //  decremented - which checks the main while loop
                    thisInput.solarMassPool -= bodyTemplate.mass;

                    if (randomizer.uniformPct() <
                        thisSpectral.pctChanceHasCompanion * pctChanceCompanionScalar)
                    {
                        bodyClassIndex = determineCompanionBodyClass(
                            bodyClassIndex,
                            common.spectralClasses,
                            common.spectralInputs,
                            randomizer
                        );
                        
                        pctChanceCompanionScalar *= 0.33;
                        adjMinSystemRadius += (minSystemRadius*0.1);
                    }
                    else
                    {
                        bodyClassIndex = -1;
                    }
                }
                
                bodyClassIndex = classIndex;        // reset to main star
                
                //  Positiion the System with radius = min(idealMax, maxRadius)
                //      If there's no room within the bounds, decrease radius
                //      until we hit the min system radius and fail.
                //
                result.result = createSystem(common, bodyClassIndex, bounds,
                        bodyTemplates,
                        std::min(adjMinSystemRadius, maxSystemRadius),
                        maxSystemRadius);
            }
        }

        ++classIndex;
    }
    
    //  finish the cell
    if (result.result == Result::kPending || result.result == Result::kRegionFull)
    {
        result.result = Result::kSuccess;
        result.entityStore = std::move(entityStore);
        result.stellarSystemTree = std::move(stellarSystemTree);
    }
    
    //  returned values that are useful for debugging
    result.starCount = common.starCount;
    
    return result;
};

ckm::scalar BuildStarmapFunction::calcBodyMass
(
    ckm::scalar maxSolarMass,
    ckm::scalar minInitialSolarMass,
    Randomizer& randomizer
)
const
{
    return minInitialSolarMass + (maxSolarMass-minInitialSolarMass)*randomizer.uniformPct() * 0.01;
}

int BuildStarmapFunction::determineCompanionBodyClass
(
    int bodyClassIndex,
    const vector<SpectralClassDesc>& spectralClasses,
    vector<SpectralInput>& spectralInputs,
    Randomizer& randomizer
)
{
    //  choose class based on two factors:
    //      number of classes available
    //      a curve where its increasingly more likely to have a lower
    //      mass companion.   
    auto roll = randomizer.uniformPct() * 0.01;
    auto classRoll = roll;
    for (auto i = 0; i < spectralClasses[bodyClassIndex].companionCurveExp; ++i)
        classRoll *= roll;
    auto availClassCount = spectralClasses.size() - bodyClassIndex;
    auto classRollInterval = 1.0/availClassCount;
    auto classRollRange = std::make_pair(0.0, 0.0);
    auto compClassIdx = availClassCount;
    for (; compClassIdx > 0; --compClassIdx)
    {
        classRollRange.second += classRollInterval;

        if (classRoll >= classRollRange.first &&
            classRoll < classRollRange.second)
    break;

        //  advance to next interval
        classRollRange.first = classRollRange.second;
    }
    // account for any math error - we always want a valid class index,
    // so always try to
    if (compClassIdx > 0)
        --compClassIdx;
    
    bodyClassIndex = bodyClassIndex+(int)compClassIdx;
    
    while (bodyClassIndex >= 0)
    {
        if (spectralInputs[bodyClassIndex].solarMassPool >=
            spectralClasses[bodyClassIndex].minInitialSolarMass)
    break;
        --bodyClassIndex;
    }

    return bodyClassIndex;
}

auto BuildStarmapFunction::createSystem
(
    CommonState& CS,
    int bodyClassIndex,
    const ckm::AABB<ckm::vec3>& bounds,
    const std::array<StarTemplate, kMaxBodiesPerSystem>& starTemplates,
    ckm::scalar minRadius,
    ckm::scalar maxRadius
)
-> Result::Enum
{
 
    auto& randomizer = CS.spectralRandomizers[bodyClassIndex];
    auto systemRadius = (maxRadius - minRadius) * randomizer.uniformPct() * 0.01;
    systemRadius += minRadius;
                
    auto systemPosition = randomPositionInWorld(CS.stellarSystemTree,
                                bounds,
                                systemRadius,
                                randomizer);
    
    if (!systemPosition.second)
    {
        systemRadius = minRadius;
        systemPosition = randomPositionInWorld(CS.stellarSystemTree,
                            bounds,
                            systemRadius,
                            randomizer);
    }
    if (systemPosition.second)
    {
        auto systemEntity = CS.entityStore.create();
        
        auto transform = CS.standardTables.transform.addDataToEntity(systemEntity);
        if (!transform)
            return Result::kOutOfMemory;
    
        transform->setLocalPosition(systemPosition.first);
        
        auto stellarSystem =
            CS.standardTables.systemTable.addDataToEntity(systemEntity);
        if (!stellarSystem)
            return Result::kOutOfMemory;
        
        stellarSystem->init(randomizer.uniformInt(), systemRadius);
        
        //  Create Bodies (with no position)
        /*
        printf("\n======================================\n");
        printf("Creating System: at=(%.5lf,%.5lf,%.5lf) radius=%lf\n",
            systemPosition.first.x, systemPosition.first.y, systemPosition.first.z,
            stellarSystem->radius);
        */
        transform->setChild(
            createBodies(CS, starTemplates, systemEntity, systemRadius)
        );
        if (!transform->child().valid())
            return Result::kOutOfMemory;

        CS.stellarSystemTree.insertObject(systemEntity);
    }
    else
    {
        return Result::kRegionFull;
    }
    
    return Result::kPending;
}

Entity BuildStarmapFunction::createBodies
(
    CommonState& state,
    const std::array<StarTemplate, kMaxBodiesPerSystem>& starTemplates,
    Entity parentSystemEntity,
    ckm::scalar systemRadius
)
{
    std::array<Entity, kMaxBodiesPerSystem> bodies;
    
    int numStars = 0;
    
    for (; numStars < starTemplates.size(); ++numStars)
    {
        auto& starTemplate  = starTemplates[numStars];
        if (starTemplate.classIndex < 0)
        {
            bodies[numStars] = Entity::null();
            break;
        }
        auto entity = state.entityStore.create();
        auto star = state.standardTables.starTable.addDataToEntity(entity);
        if (!star)
            return Entity::null();
        
        //  star props
        star->init(starTemplate.mass,
            massToRadius(starTemplate.mass),
            massToLuminosity(starTemplate.mass)
        );
        /*
         printf("Creating Star: temp=%u, mass=%lf, radius=%lf, luminosity=%lf, Mv=%lf\n",
            star->effectiveTemp,
            star->solarMass,
            star->solarRadius,
            star->solarLuminosity,
            star->visualMag);
        */
    
        bodies[numStars] = entity;
    }
    
    if (!numStars)
        return Entity::null();      // bad input
    
    //  fixup each entity's sibling links and determine starting offsets from the
    //  center of mass.
    
    // use AU as our basic unit,  converting to our LY unit once finished
    // calculating of center of mass.
    ckm::scalar totalMass = 0;
    ckm::scalar totalMassMulOffset = 0;
    
    std::array<ckm::scalar, kMaxBodiesPerSystem> offsets;
    const ckm::scalar kOffsetLimit = systemRadius * kSystemInnerRadiusFraction;
    std::pair<ckm::scalar, ckm::scalar> offsetRange = { 0, 0 };
    
    int offsetFraction = 1;
    for (int i = 1; i < numStars; ++i)
        offsetFraction *= 10;
    
    for (int starIndex = 0; starIndex < numStars; ++starIndex)
    {
        Entity entity = bodies[starIndex];
        
        auto transform = state.standardTables.transform.addDataToEntity(entity);
        if (!transform)
            return Entity::null();
        
        transform->setParent(parentSystemEntity);
        if (starIndex < bodies.size()-1)
        {
            transform->setSibling(bodies[starIndex+1]);
        }
        
        auto star = state.standardTables.starTable.dataForEntity(entity);
        
        totalMass += star->solarMass();
        totalMassMulOffset += offsetRange.first*star->solarMass();
        
        offsets[starIndex] = offsetRange.first;
        
        auto& randomizer = state.spectralRandomizers[starTemplates[starIndex].classIndex];
        
        offsetRange.first = offsetRange.first +
            (star->solarRadius() + kMaxStarPairDistInSolRadius * randomizer.uniformPct()*0.01)/kSolRadiusPerAU;
        
        //  if numStars == 1, this block is never executed, and numStars > 2
        //      then offsetFraction will be 10^(numStars)
        if (starIndex+1 < numStars)
        {
            offsetFraction /= 10;
            offsetRange.second = (kOffsetLimit/offsetFraction) * kAUPerLYR;
            
            auto baseModifier = randomizer.uniformPct()/100;
            auto modifier = baseModifier;
            for (int i = starIndex; i < kMaxBodiesPerSystem; ++i)
            {
                modifier *= baseModifier;
            }
            offsetRange.first += (offsetRange.second - offsetRange.first) * modifier;
        }
        
    }
    
    //  now calculate center of mass and offset every body by the center
    //  so that offset 0 = center of the system
    auto starOffset = totalMassMulOffset/totalMass;
    for (int starIndex = 0; starIndex < numStars; ++starIndex)
    {
        auto transform = state.standardTables.transform.dataForEntity(bodies[starIndex]);
        
        ckm::scalar lyOffset = (offsets[starIndex] - starOffset) / kAUPerLYR;
        
        ckm::vec3 offset(lyOffset, 0, 0);
        transform->setLocalPosition(offset);
    }
    
    state.starCount += numStars;
    
    return bodies[0];
}
    
//  a variant of the equations here:
//  http://www.astro.caltech.edu/~george/ay20/eaa-stellarmasses.pdf
//
//  using the form L = K*(M^a)
//      where "a" relies on a main sequence mass range
//      and K is an adjustment based on its apparent position on an H-R
//      diagram (grows as mass increases.)
//
ckm::scalar BuildStarmapFunction::massToLuminosity
(
    ckm::scalar solarMass
)
const
{
    if (solarMass < 0.43)
    {
        return 0.23 * std::pow(solarMass, 2.3);
    }
    else if (solarMass < 2.0)
    {
        return std::pow(solarMass, 4);
    }
    else if (solarMass < 20.0)
    {
        return 1.5 * std::pow(solarMass, 3.5);
    }
    else
    {
        return 3200.0 * solarMass;
    }
}

ckm::scalar BuildStarmapFunction::massToRadius(ckm::scalar solarMass) const
{
    return std::pow(solarMass, 0.8);
}


std::pair<ckm::vec3, bool> BuildStarmapFunction::randomPositionInWorld
(
    const StellarSystemTree& world,
    const ckm::AABB<ckm::vec3>& bounds,
    ckm::scalar systemRadius,
    Randomizer& randomizer
)
{
    ckm::vec3 pos;
    
    int intersectCounter = 0;
    constexpr int kMaxIntersects = 8;
    
    auto test = world.test<StellarSystemTree::Test::IntersectWithSphere>();
    
    do
    {
        pos = randomPositionWithinBounds(randomizer, bounds);
        
        if (test(pos,systemRadius))
        {
            ++intersectCounter;
        }
        else
        {
            intersectCounter = 0;
        }
    }
    while(intersectCounter > 0 && intersectCounter < kMaxIntersects);
    
    return std::make_pair(pos, intersectCounter <= 0);
}

ckm::vec3 BuildStarmapFunction::randomPositionWithinBounds
(
    Randomizer& randomizer,
    const ckm::AABB<ckm::vec3>& bounds
)
const
{
    ckm::vec3 pos;
    pos.x = std::generate_canonical<decltype(pos)::value_type, 6>(randomizer.rand);
    pos.y = std::generate_canonical<decltype(pos)::value_type, 6>(randomizer.rand);
    pos.z = std::generate_canonical<decltype(pos)::value_type, 6>(randomizer.rand);
    
    //  prevent having a system "on the edge"
    //  also handles an implementation bug:
    //      "Some existing implementations have a bug where they may
    //       occasionally return 1.0 if RealType is float GCC #63176
    //       LLVM #18767"
    //  http://en.cppreference.com/w/cpp/numeric/random/generate_canonical
    
    const ckm::scalar kMin = (ckm::scalar)0.1;
    const ckm::scalar kMax = (ckm::scalar)0.99;
    
    if (pos.x < kMin)
        pos.x = kMin;
    if (pos.y < kMin)
        pos.y = kMin;
    if (pos.z < kMin)
        pos.z = kMin;
    
    if (pos.x > kMax)
        pos.x = kMax;
    if (pos.y > kMax)
        pos.y = kMax;
    if (pos.z > kMax)
        pos.z = kMax;
    
    pos *= bounds.dimensions();
    pos += bounds.min;
    
    return pos;
}
    
} /* namespace ovproto */ } /* namespace cinek */