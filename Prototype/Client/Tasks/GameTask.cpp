//
//  GameTask.cpp
//  Overview
//
//  Created by Samir Sinha on 6/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "GameTask.hpp"

//  used for building a galactic "cell"
#include "Engine/Graphs/BVHGraph.hpp"

#include <cinek/vector.hpp>
#include <random>


namespace cinek {

using namespace overview;

namespace ovproto {

/*
    The Build Cell Algorithm
    ----------------------------------------------------------------------------
    
    Summary:    Generate stars within a bounding volume.
    Objective:  Generate stars of sufficient variety and distribution within a
                bounding volume, depending on certain astronomical input
                properties.
                
    Parameters: seed            - Used for randomization
                bounds          - Min/Max Bounds within the world
                ageInYears      - Age of the galaxy.  Used to determine the
                                  ratio of main sequence to late/end-stage
                                  body counts
                solarMass       - Solar mass of matter in the cell
                spectralClasses - Vector of body classes for main sequence stars
                
    Algorithm
    ----------------------------------------------------------------------------
    
    I)  Evaluate whether we're done
    
        - Are we out of solar mass?  If so then exit.
 
    II) Creating a system
        
        - Determine solar mass used by system.
        - Specify a starting main sequence age for the system.
        - Create one or more bodies from the solar mass
            - Determine spectral class of body based on specified percentages
            - Determine main-sequence age of body
            - Adjust class of body based on age of galaxy and our main sequence
              age
            - Return solar mass to the system after adjusting for age
            - Adjust age accordingly for the body
        - Generate a seed for our system.
    
    III) Placing a system
    
    IV) Loop back to I
*/

struct SpectralClass
{
    int32_t minEffectiveTemp;           // in Kelvin
    double minSolarMass;                // minimum solar mass
    double minSolarRadius;              // minimum solar radius
    double minSolarLuminosity;          // minimum absolute magnitude
    double pctChanceIsCompanion;        // change body is a companion of another
    enum { kWeibull, kUniform } distFn; // distrivution function of
};

struct BuildCellFunction
{
public:
    using GraphType = BVHTree;
    
    //  parameters for generating a sequence of bodies per spectral class
    //  this
    struct SpectralInput
    {
        double solarMassPool;
    };

    //  A single output of the build function
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
        Vector3 offset;                 // offset from system center position
        int32_t effectiveTemp;          // i.e. 5600K, etc
        double solarRadius;             // Radius in Sol units
        double solarMass;               // Radius in Sol mass
        double luminosity;              // Bolometric luminosity
        double ageInYears;              // age of the body in years
        double timeToNextStageInYears;  // how long before next stage begins
        int32_t systemIndex;            // what system this belongs to (if any)
        int32_t siblingBodyIndex;       // body shares the system with this
    };
    
    struct System
    {
        Vector3 position;               // absolute position in space
        uint32_t systemSeed;            // random seed used to generate system
        int32_t firstBodyIndex;         // first body in the system
        intptr_t graphIndex;            // index on the cell graph (-1 if invalid)
    };
    
    struct Cell
    {
        using Graph = BuildCellFunction::GraphType;
        vector<Body> bodies;
        vector<System> systems;
        enum { kSuccess, kPending, kInvalidInput, kOutOfMemory } result;
        Graph graph;
    };
    
    static constexpr size_t kMaxSystems = 32;
    static constexpr size_t kMaxBodies = kMaxSystems*4;
    static constexpr double kMaxSystemRadius = 0.5;       // within one lyr
    
    Cell operator()
    (
        uint32_t masterSeed,
        const vector<SpectralClass>& spectralClasses,
        vector<SpectralInput> spectralInputs,
        int spectralLevel,
        AABB<Vector3> bounds,
        double ageInYears
    )
    {
        Cell cell;
        cell.result = Cell::kPending;
        
        //  terminate if inputs are incorrect or we're out of memory
        if (spectralClasses.size() != spectralInputs.size())
        {
            cell.result = Cell::kInvalidInput;
            return cell;
        }
        cell.systems.reserve(kMaxSystems);
        if (cell.systems.capacity() != kMaxSystems)
        {
            cell.result = Cell::kOutOfMemory;
            return cell;
        }
        cell.bodies.reserve(kMaxBodies);
        if (cell.bodies.capacity() != kMaxBodies)
        {
            cell.result = Cell::kOutOfMemory;
            return cell;
        }
        
        ////////////////////////////////////////////////////////////////////////
        GraphUtility graphUtility;
        graphUtility.cell = &cell;
        
        GraphType objectTree(kMaxBodies*8);
        
        //  todo - generate seeds from masterSeed for common cell simulation
        //  params
        std::minstd_rand masterRand(masterSeed);
        std::uniform_int_distribution<uint32_t> seederDist(1,5);
        std::array<uint32_t, 3> seeders;
        for (auto& s : seeders)
        {
            s = seederDist(masterRand);
        }
        
        vector<uint32_t> spectralSeeds(spectralClasses.size());
        std::seed_seq seeder(seeders.begin(), seeders.end());
        seeder.generate(spectralSeeds.begin(), spectralSeeds.end());
        
        //  generate stellar bodies for every configured spectral class
        //  every class has its own seed used for body generation
        //
        
        int classIndex = 0;
        
        for (auto& spectralInput : spectralInputs)
        {
            if (cell.result != Cell::kPending)
                break;
            
            if (classIndex > spectralLevel)
                continue;
         
            auto& spectralClass = spectralClasses[classIndex];
         
            //  spectral class 0 is 'max possible star class + 1' reference so
            //  we can have min/max ranges for various attributes.
            if (classIndex > 0)
            {
                //  random generator for this spectral class
                std::minstd_rand spectralRand(spectralSeeds[classIndex]);
                std::weibull_distribution<> weibullDist;
                std::uniform_real_distribution<> uniformDist;
                std::uniform_int_distribution<> uniformIntDist;
             
                double solarMassAvail = spectralInput.solarMassPool;
                auto& prevSpectralClass = spectralClasses[classIndex-1];

                const double kAgeMultiplier = ageInYears/(uniformIntDist.max() - uniformIntDist.min());

                //  generate as many bodies as we can for this spectral class
                while (solarMassAvail >= spectralClass.minSolarMass)
                {
                    ////////////////////////////////////////////////////////////
                    //  CREATE SYSTEM
                    auto systemIndex = allocate(cell.systems);
                    if (systemIndex < 0)
                    {
                        cell.result = Cell::kOutOfMemory;
                        break;
                    }
                    
                    //  CREATE BODY as a zero age main sequence star.
                    auto bodyIndex = allocate(cell.bodies);
                    if (bodyIndex < 0)
                    {
                        cell.result = Cell::kOutOfMemory;
                        break;
                    }
                    
                    auto& system = cell.systems[systemIndex];
                    system.systemSeed = spectralRand();
                    system.firstBodyIndex = -1;
                    
                    //  POSITION SYSTEM
                    auto plotResult = plotSystemOnGraph(systemIndex, system,
                        kMaxSystemRadius,
                        bounds,
                        objectTree,
                        spectralRand,
                        graphUtility);
                    
                    if (!plotResult)
                    {
                        system.graphIndex = -1;
                        break;
                    }
                    
                    ////////////////////////////////////////////////////////////
                    //  BODY

                    auto& body = cell.bodies[bodyIndex];
                    
                    
                    //  MASS
                    //      determine mass to use - this mass is the mass at the
                    //      start of the main sequence

                    double scalar;  // 0 to 1
                    if (spectralClass.distFn == SpectralClass::kWeibull)
                    {
                        scalar = weibullDist(spectralRand);
                        scalar = (scalar + weibullDist.min()) / (weibullDist.max() - weibullDist.min());
                    }
                    else
                    {
                        scalar = uniformDist(spectralRand);
                    }
                    
                    body.solarMass = (prevSpectralClass.minSolarMass - spectralClass.minSolarMass)*scalar;
                    body.solarMass += spectralClass.minSolarMass;
                    if (solarMassAvail < body.solarMass)
                    {
                        body.solarMass = solarMassAvail;
                    }
                    
                    //  using this mass, we can begin to calculate other solar
                    //  properties like...
                    
                    //  LUMINOSITY
                    body.luminosity = massToLuminosity(body.solarMass, 1.0);

                    //  DETERMINE EVOLUTIONARY STAGE
                    scalar = uniformDist(spectralRand);
                    body.ageInYears = uniformIntDist(spectralRand) * scalar * kAgeMultiplier;
                    body.stage = Body::kMainSequence;
                
                    //  Position our star
                    body.offset.comp[0] = 0.f;
                    body.offset.comp[1] = 0.f;
                    body.offset.comp[2] = 0.f;

                    body.systemIndex = systemIndex;
                    body.siblingBodyIndex = -1;
                    
                    system.firstBodyIndex = bodyIndex;
                    
                    //  reduce our available mass for body generation
                    solarMassAvail -= body.solarMass;
                }
                spectralInput.solarMassPool = solarMassAvail;
            }

            ++classIndex;
        }
        
        //  finish the cell
        if (cell.result == Cell::kPending)
        {
            cell.result = Cell::kSuccess;
            cell.graph = std::move(objectTree);
        }
        
        return cell;
    };
    
private:
    struct GraphUtility
    {
        Cell* cell;

        //objId = system
        void setObjectData(intptr_t objId, intptr_t data)
        {
            auto& system = cell->systems[objId];
            system.graphIndex = data;
        }
        
        float objectRadius(intptr_t ) const
        {
            return BuildCellFunction::kMaxSystemRadius;
        }
        
        Vector3 position(intptr_t objId) const
        {
            auto& system = cell->systems[objId];
            return system.position;
        }
    };
    
    template<typename _Container>
    int allocate(_Container& container) const
    {
        auto index = container.size();
        container.emplace_back();
        if (index == container.size())
            return -1;
        return (int)index;
    }
    
    //  a variant of the equations here:
    //  https://en.wikipedia.org/wiki/Mass%E2%80%93luminosity_relation
    //
    //  using the form L = K*(M^a)
    //      where "a" relies on a main sequence mass range
    //      and K is an adjustment based on its apparent position on an H-R
    //      diagram (grows as mass increases.)
    //
    //      This method adjusts the base 'K' as a star's luminosity tends
    //      to grow significantly as the solar mass increases into the B and O
    //      type ranges
    //
    //      To account for this, 'K' will be augmented by a supplied multipler
    //
    double massToLuminosity(double solarMass, double multiplier)
    {
        if (solarMass < 0.43)
        {
            return 0.23 * std::pow(solarMass, 2.3);
        }
        else if (solarMass < 2.0)
        {
            return multiplier * std::pow(solarMass, 4);
        }
        else if (solarMass < 20.0)
        {
            return multiplier * 1.5 * std::pow(solarMass, 3.5);
        }
        else
        {
            return multiplier * 3200.0 * solarMass;
        }
    }
    
    template<typename _Randomizer, typename _Utility>
    bool plotSystemOnGraph
    (
        intptr_t systemIndex,
        System& system,
        const double maxDensity,
        const AABB<Vector3>& bounds,
        GraphType& graph,
        _Randomizer& randomizer,
        _Utility& util
    )
    {
        Vector3 pos;
        
        Vector3 limits = bounds.dimensions();
        int intersectCounter = 0;
        constexpr int kMaxIntersects = 16;
        
        do
        {
            pos.comp[0] = std::generate_canonical<float, 6>(randomizer);
            pos.comp[1] = std::generate_canonical<float, 6>(randomizer);
            pos.comp[2] = std::generate_canonical<float, 6>(randomizer);
            
            //  prevent having a system "on the edge"
            //  also handles an implementation bug:
            //      "Some existing implementations have a bug where they may
            //       occasionally return 1.0 if RealType is float GCC #63176
            //       LLVM #18767"
            //  http://en.cppreference.com/w/cpp/numeric/random/generate_canonical
            
            if (pos.comp[0] < 0.01f)
                pos.comp[0] = 0.01f;
            if (pos.comp[1] < 0.01f)
                pos.comp[1] = 0.01f;
            if (pos.comp[2] < 0.01f)
                pos.comp[2] = 0.01f;
            
            if (pos.comp[0] > 0.98f)
                pos.comp[0] = 0.98f;
            if (pos.comp[1] > 0.98f)
                pos.comp[1] = 0.98f;
            if (pos.comp[2] > 0.98f)
                pos.comp[2] = 0.98f;
            

            pos *= limits;
            pos += bounds.min;
            
            if (graph.testIntersectWithSphere(pos, maxDensity))
            {
                ++intersectCounter;
            }
            else
            {
                intersectCounter = 0;
            }
        }
        while(intersectCounter > 0 && intersectCounter < kMaxIntersects);
        
        if (intersectCounter > 0)
            return false;
        
        system.position = pos;
        
        graph.insertObject(systemIndex, util);
        
        return true;
    }
};


GameTask::GameTask(AppInterface api) :
    _API(api),
    _bodyEntityId { 0 }
{
}

void GameTask::onBegin()
{
    _API.createJsonDocumentFromFile(kDocumentId_EntityTemplates, "entity.json");
    
    _bodyEntityId = _API.createEntity(
        kDocumentId_EntityTemplates,
        "star"
    );
    
    //  position our new star entity - note, this is a brute force method that
    //  bypasses any notification, etc.  use a proper physics system or add
    //  a utility to manipulate entities that sends out the appropriate
    //  notification
    
    /*
    auto& entityMatrix = entityObj->matrix();
    bx::mtxSRT(entityMatrix, 2.0f, 2.0f, 2.0f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f);
    */
    
    BuildCellFunction cellGenFn;
    
    vector<SpectralClass> spectralClasses =
    {
        {   55000, 275.0,  38.0, 1000000,    0.0,  SpectralClass::kWeibull  },
        {   30000,  16.0,   6.6,   30000,    0.0,  SpectralClass::kWeibull  },
        {   10000,   2.1,   1.8,      25,    1.0,  SpectralClass::kUniform  },
        {   7500,    1.4,   1.4,       5,    5.0,  SpectralClass::kUniform  },
        {   6000,   1.04,  1.15,     1.5,    10.0, SpectralClass::kUniform  },
        {   5200,    0.8,  0.96,     0.6,    25.0, SpectralClass::kUniform  },
        {   3700,   0.45,   0.7,    0.08,    25.0, SpectralClass::kUniform  },
        {   2400,   0.08,   0.1,  5.3e-6,    33.0, SpectralClass::kUniform  }
    };
    /*
    vector<BuildCellFunction::SpectralInput> spectralInputs =
    {
        { 0.0 },
        { 0.00003 },
        { 0.13 },
        { 0.6 },
        { 3.0 },
        { 7.6 },
        { 12.1 },
        { 76.45 }
    };
    */
    constexpr double kSolarMassTotal = 20.0;
    vector<BuildCellFunction::SpectralInput> spectralInputs =
    {
        { 0.0 },
        { kSolarMassTotal * 0.0 },
        { kSolarMassTotal * 0.01 },
        { kSolarMassTotal * 0.09 },
        { kSolarMassTotal * 0.25 },
        { kSolarMassTotal * 0.35 },
        { kSolarMassTotal * 0.20 },
        { kSolarMassTotal * 0.10 }
    };
    Vector3 cellMin = {{ -8,-8,-8 }};
    Vector3 cellMax = {{ 8, 8, 8 }};
    AABB<Vector3> cellBounds(cellMin, cellMax);
    
    auto cell = cellGenFn(10245,
        spectralClasses,
        spectralInputs,
        7,
        cellBounds,
        12.0e9);

    //  create entities from cell
    //  iterate through systems, creating entities from systems and bodies
    

    //bx::mtxScale(mtx, 2.0f, 2.0f, 2.0f);
    //bx::mtxMul(entityTransform->matrix, entityTransform->matrix, mtx);
    //bx::mtxTranslate(mtx, 0.0f, 0.0f, 0.0f);
    //bx::mtxMul(entityTransform->matrix, entityTransform->matrix, mtx);
    
    /*
    entityObj = _API.createEntity(
        kDocumentId_EntityTemplates,
        "star"
    );
    
    entityTransform = &entityObj->transform();
    bx::mtxRotateXYZ(entityTransform->matrix, 0.0f, 0.0f, bx::pi);
    bx::mtxTranslate(mtx, 2.f, 2.f, 0.f);
    bx::mtxMul(entityTransform->matrix, entityTransform->matrix, mtx);
    */
}

void GameTask::onUpdate(uint32_t deltaTimeMs)
{
    Matrix4 rotMat;
    //bx::mtxIdentity(rotMat);
    bx::mtxRotateXYZ(rotMat, 0.f, bx::piHalf*0.10f*deltaTimeMs/1000.f, 0.f);
    
    /*
    auto object = _API.entityById(_bodyEntityId);
    if (object)
    {
        auto& entityMatrix = object->matrix();
        Matrix4 transform;
        
        bx::mtxMul(transform, entityMatrix, rotMat);
        object->matrix() = transform;
    }
    */
}

void GameTask::onEnd()
{
}

void GameTask::onFail()
{
}


} /* namespace ovproto */ } /* namespace cinek */
