//
//  AIGameClient.hpp
//  Overview
//
//  Created by Samir Sinha on 8/6/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Sim_AIGameClient_hpp
#define Overview_Sim_AIGameClient_hpp

#include "AIObject.hpp"

namespace cinek { namespace ovproto {

class AIGameClient
{
public:
    void begin(AIState& state, AIObject& object);
    void simulate(AIState& state, AIObject& object);
    void end(AIState& state, AIObject& object);
    void suspend(AIState& state, AIObject& object);
    void resume(AIState& state, AIObject& object);
};

} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_Sim_AIGameClient_hpp) */
