//
//  BuildStarmap.cpp
//  Overview
//
//  Created by Samir Sinha on 6/29/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "BuildStarmap.hpp"

#include "Engine/Entity/EntityStore.hpp"
#include "Engine/Entity/Comp/Transform.hpp"
#include "Engine/Entity/Comp/EntityHierarchy.hpp"
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
        return v * 100.0;
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
    const vector<SpectralClass>& spectralClasses;
    vector<SpectralInput>& spectralInputs;
    int spectralIndexMax;
    vector<Randomizer>& spectralRandomizers;
    simtime ageInYears = 0;
    
    overview::EntityStore& entityStore;
    StellarSystemTree& stellarSystemTree;
    StandardTables& standardTables;
    
    CommonState(const vector<SpectralClass>& specClasses,
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
        standardTables(tables)
    {
    }
};

    
auto BuildStarmapFunction::operator()
(
    uint32_t masterSeed,
    const vector<SpectralClass>& spectralClasses,
    vector<SpectralInput> spectralInputs,
    int spectralIndexMax,
    AABB<ckm::vec3> bounds,
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
    overview::EntityStore entityStore(64*1024, {
        { overview::component::Transform::kComponentType, 64*1024 },
        { overview::component::EntityHierarchy::kComponentType, 64*1024 },
        { ovproto::component::StellarSystem::kComponentType, 16*1024 },
        { ovproto::component::StarBody::kComponentType, 48*1024 }
    });
    
    StandardTables standardTables;
    standardTables.transform = entityStore.table<overview::component::Transform>();
    standardTables.hierarchy = entityStore.table<overview::component::EntityHierarchy>();
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
            
            while (spectralInput.solarMassPool >= spectralClass.minSolarMass &&
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
                    
                    bodyTemplate.mass = calcBodyMass(prevSpectral.minSolarMass,
                        thisSpectral.minSolarMass,
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
    if (result.result == Result::kPending)
    {
        result.result = Result::kSuccess;
        result.entityStore = std::move(entityStore);
        result.stellarSystemTree = std::move(stellarSystemTree);
    }
    
    return result;
};

ckm::scalar BuildStarmapFunction::calcBodyMass
(
    ckm::scalar maxSolarMass,
    ckm::scalar minSolarMass,
    Randomizer& randomizer
)
const
{
    return minSolarMass + (maxSolarMass-minSolarMass)*randomizer.uniformPct() * 0.01;
}

int BuildStarmapFunction::determineCompanionBodyClass
(
    int bodyClassIndex,
    const vector<SpectralClass>& spectralClasses,
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
            spectralClasses[bodyClassIndex].minSolarMass)
    break;
        --bodyClassIndex;
    }

    return bodyClassIndex;
}

auto BuildStarmapFunction::createSystem
(
    CommonState& CS,
    int bodyClassIndex,
    const AABB<ckm::vec3>& bounds,
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
        
        auto transform = CS.standardTables.transform.addComponentToEntity(systemEntity);
        if (!transform)
            return Result::kOutOfMemory;
    
        transform->setLocalPosition(systemPosition.first);
        
        auto stellarSystem =
            CS.standardTables.systemTable.addComponentToEntity(systemEntity);
        if (!stellarSystem)
            return Result::kOutOfMemory;
        
        stellarSystem->radius = systemRadius;
        stellarSystem->seed = randomizer.uniformInt();
        
        //  Create Bodies (with no position)
        auto hierarchy = CS.standardTables.hierarchy.addComponentToEntity(systemEntity);
        if (!hierarchy)
            return Result::kOutOfMemory;
        
        printf("\n======================================\n");
        printf("Creating System: radius=%lf\n", stellarSystem->radius);
        
        hierarchy->firstChild = createBodies(CS, starTemplates, systemEntity, systemRadius);
        if (!hierarchy->firstChild)
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
    
    for (int i = 0; i < starTemplates.size(); ++i)
    {
        auto& starTemplate  = starTemplates[i];
        if (starTemplate.classIndex < 0)
        {
            bodies[i] = Entity::null();
            break;
        }
        auto entity = state.entityStore.create();
        auto transform = state.standardTables.transform.addComponentToEntity(entity);
        if (!transform)
            return Entity::null();
        
        auto hierarchy = state.standardTables.hierarchy.addComponentToEntity(entity);
        if (!hierarchy)
            return Entity::null();
        
        auto star = state.standardTables.starTable.addComponentToEntity(entity);
        if (!star)
            return Entity::null();
        
        ckm::vec3 offset(0.0);
        
        //  transformation props
        transform->setLocalPosition(offset);
    
        //  star props
        star->solarMass = starTemplate.mass;
        star->solarLuminosity = massToLuminosity(star->solarMass);
        star->solarRadius = massToRadius(star->solarMass);
        star->effectiveTemp = radiusAndLuminosityToEffTemp(star->solarRadius, star->solarLuminosity);
       
        printf("Creating Star: temp=%u, mass=%lf, radius=%lf, luminosity=%lf\n",
            star->effectiveTemp,
            star->solarMass,
            star->solarRadius,
            star->solarLuminosity);
    
       
        bodies[i] = entity;
    }
    
    //  fixup each entity's sibling links
        
    for (int bodyIndex = 0; bodyIndex < bodies.size(); ++bodyIndex)
    {
        if (!bodies[bodyIndex])
            break;

        auto hierarchy = state.standardTables.hierarchy.rowForEntity(bodies[bodyIndex]);
    
        hierarchy->parent = parentSystemEntity;
        
        if (bodyIndex < bodies.size()-1)
        {
            hierarchy->nextSibling = bodies[bodyIndex+1];
        }
        else
        {
            hierarchy->nextSibling = Entity::null();
        }
    }

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

//  converts radius & lumniosity to effective temp
ckm::scalar BuildStarmapFunction::radiusAndLuminosityToEffTemp
(
    ckm::scalar radius,
    ckm::scalar luminosity
)
const
{
    //  ((R^2)/L) ^ (1/4) = 1/Teff
    //  http://skyserver.sdss.org/dr1/en/proj/advanced/hr/radius1.asp
    auto tempRatio = std::sqrt((radius*radius)/luminosity); // ratio is squared
    tempRatio = std::sqrt(tempRatio);
    tempRatio = 1/tempRatio;
    
    return tempRatio * kSolarEffTemp;
}

std::pair<ckm::vec3, bool> BuildStarmapFunction::randomPositionInWorld
(
    const StellarSystemTree& world,
    const AABB<ckm::vec3>& bounds,
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
    const AABB<ckm::vec3>& bounds
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
    
    if (pos.x < 0.01)
        pos.x = 0.01;
    if (pos.y < 0.01)
        pos.y = 0.01;
    if (pos.z < 0.01)
        pos.z = 0.01;
    
    if (pos.x > 0.99)
        pos.x = 0.99;
    if (pos.y > 0.99)
        pos.y = 0.99;
    if (pos.z > 0.99)
        pos.z = 0.99;
    
    pos *= bounds.dimensions();
    pos += bounds.min;
    
    return pos;
}
    
} /* namespace ovproto */ } /* namespace cinek */