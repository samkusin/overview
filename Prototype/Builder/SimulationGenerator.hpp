//
//  SimulationGenerator.h
//  Overview
//
//  Created by Samir Sinha on 10/17/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Builder_SimulationGenerator_hpp
#define Overview_Builder_SimulationGenerator_hpp

#include "cinek/allocator.hpp"

namespace cinekine { namespace ovengine {
    class GameTemplates;
} /* namespace ovengine */ } /* namespace cinekine */

namespace cinekine { namespace prototype {
    
    class Simulation;
    
    struct CreateSimulationParams
    {
        uint32_t entityLimit = 32;
        Allocator allocator = Allocator();
    };
    
    unique_ptr<Simulation> generateSimulation(const ovengine::GameTemplates& gameTemplates,
                                              const CreateSimulationParams& params);
    
} /* namespace prototype */ } /* namespace cinekine */


#endif /* ifndef Overview_Builder_SimulationGenerator_hpp */
