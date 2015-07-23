//
//  BuildStarmap.hpp
//  Overview
//
//  Created by Samir Sinha on 6/29/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Sim_BuildStarmap_hpp
#define Overview_Sim_BuildStarmap_hpp

#include "GameTypes.hpp"
#include "SimDefines.hpp"
#include "StellarSystemUtility.hpp"
#include "Engine/Entity/EntityDataTable.hpp"
#include "Engine/Entity/EntityStore.hpp"

#include <cinek/vector.hpp>
#include <array>

namespace cinek {

namespace ovproto {

/*
    The Build Starmap Algorithm
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

struct BuildStarmapFunction
{
private:
    struct StandardTables
    {
        component::Table<overview::component::Transform> transform;
        component::Table<component::StellarSystem> systemTable;
        component::Table<component::StarBody> starTable;
    };
    
    struct Randomizer;
    struct CommonState;
    
public:
    //  parameters for generating a sequence of bodies per spectral class
    //  this
    struct SpectralInput
    {
        ckm::scalar solarMassPool;
    };
    
    struct Result
    {
        enum Enum
        {
            kSuccess,
            kPending,
            kInvalidInput,
            kOutOfMemory,
            kRegionFull
        };
        Enum result;
        
        overview::EntityStore entityStore;
        StellarSystemTree stellarSystemTree;
        int starCount;
    };
    
    static constexpr auto kBaseSystemRadius = 0.10; // lyr
    
    Result operator()
    (
        uint32_t masterSeed,
        const vector<SpectralClassDesc>& spectralClasses,
        vector<SpectralInput> spectralInputs,
        int spectralIndexMax,
        ckm::AABB<ckm::vec3> bounds,
        ckm::scalar minSystemRadius,
        ckm::scalar maxSystemRadius
    );
    
private:
    struct StarTemplate
    {
        ckm::scalar mass;
        int classIndex;
    };
    
    static constexpr int kMaxBodiesPerSystem = kMaxStarsPerSystem;

    ckm::scalar calcBodyMass(ckm::scalar maxSolarMass,
        ckm::scalar minInitialSolarMass,
        Randomizer& randomizer) const;
    
    int determineCompanionBodyClass(int bodyClassIndex,
        const vector<SpectralClassDesc>& spectralClasses,
        vector<SpectralInput>& spectralInputs,
        Randomizer& randomizer);
    
    Result::Enum createSystem(CommonState& state, int bodyClassIndex,
        const ckm::AABB<ckm::vec3>& bounds,
        const std::array<StarTemplate, kMaxBodiesPerSystem>& starTemplates,
        ckm::scalar minRadius,
        ckm::scalar maxRadius);
    
    Entity createBodies(CommonState& state,
        const std::array<StarTemplate, kMaxBodiesPerSystem>& starTemplates,
        Entity parentSystemEntity,
        ckm::scalar systemRadius);
    
    std::pair<ckm::vec3, bool> randomPositionInWorld
    (
        const StellarSystemTree& world,
        const ckm::AABB<ckm::vec3>& bounds,
        ckm::scalar systemRadius,
        Randomizer& randomizer
    );
  
    ckm::vec3 randomPositionWithinBounds(Randomizer& randomizer,
        const ckm::AABB<ckm::vec3>& bounds) const;

    //  a variant of the equations here:
    //  https://en.wikipedia.org/wiki/Mass%E2%80%93luminosity_relation
    //
    //  using the form L = K*(M^a)
    //      where "a" relies on a main sequence mass range
    //      and K is an adjustment based on its apparent position on an H-R
    //      diagram (grows as mass increases.)
    //
    ckm::scalar massToLuminosity(ckm::scalar solarMass) const;
    
    //  converts mass to radius for a main sequence star at zero age
    ckm::scalar massToRadius(ckm::scalar solarMass) const;
    
    //  converts radius & lumniosity to effective temp
    ckm::scalar radiusAndLuminosityToEffTemp(ckm::scalar radius, ckm::scalar luminosity) const;
};


} /* namespace ovproto */

} /* namespace cinek */

#endif /* defined(Overview_Sim_BuildStarmap_hpp) */
