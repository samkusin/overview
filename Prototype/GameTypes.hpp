//
//  SimulationTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 11/22/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_Types_hpp
#define Overview_Game_Types_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/MessageTypes.hpp"
#include "Engine/Entity/EntityTypes.hpp"
#include "Engine/AABB.hpp"

namespace cinek {
    namespace ovproto {
    
    using simtime = double;
    
    //  bring certain Engine types over into our namespace
    using Entity = overview::Entity;
          
    class AppInterface;
    class AppContext;
    
    
    enum
    {
        kViewControllerId_Galaxy
    };
    
    } /* namespace overview */
} /* namespace cinek */

#endif
