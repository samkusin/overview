//
//  AIActionClient.hpp
//  Overview
//
//  Created by Samir Sinha on 8/14/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Sim_AIActionSystem_hpp
#define Overview_Sim_AIActionSystem_hpp

#include "Simulation.hpp"
#include "ActionTypes.hpp"

namespace cinek { namespace ovproto {

class AIActionClient
{
public:
    bool begin(Action& action, double time);
    bool update(Action& action, double time, double dt);
    void abort(Action& action, double time);
    Action::return_type end(Action& action, double time);
};


} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_Sim_AIActionSystem_hpp) */
