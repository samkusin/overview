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

namespace cinek { namespace ovproto {

struct AIAction
{
    using key_type = Entity;
    using return_type = uint32_t;
    
    key_type id() const {
        return _id;
    }
    
    void setId(key_type id) {
        _id = id;
    }
    
private:
    key_type _id;
};

class AIActionClient
{
public:
    bool begin(AIAction& action, double time);
    bool update(AIAction& action, double time, double dt);
    AIAction::return_type end(AIAction& action, double time);
};


} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_Sim_AIActionSystem_hpp) */
