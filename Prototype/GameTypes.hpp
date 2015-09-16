//
//  GameTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 11/22/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_Types_hpp
#define Overview_Game_Types_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/MessageTypes.hpp"

namespace cinek {
    namespace ovproto {
    
    using simtime = double;
    
    //  prototypes
    class AppInterface;
    class AppContext;
    
    //  game states
    enum
    {
        kViewControllerId_Galaxy
    };
    
    } /* namespace overview */
} /* namespace cinek */

#endif
