//
//  SimulationGenerator.hpp
//  Overview
//
//  Created by Samir Sinha on 10/17/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Builder_SimulationGenerator_hpp
#define Overview_Builder_SimulationGenerator_hpp

#include "cinek/allocator.hpp"

namespace cinek { namespace overview {
    class GameTemplates;
    class StaticWorldMap;
} /* namespace overview */ } /* namespace cinek */

namespace cinek { namespace overview {

    class Simulation;

    struct CreateSimulationParams
    {
        Allocator allocator = Allocator();
        const StaticWorldMap* staticWorldMap = nullptr;
    };

    unique_ptr<Simulation> generateSimulation(const overview::GameTemplates& gameTemplates,
                                              const CreateSimulationParams& params);

} /* namespace overview */ } /* namespace cinek */


#endif /* ifndef Overview_Builder_SimulationGenerator_hpp */
