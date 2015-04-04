//
//  SimulationTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 11/22/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_Types_hpp
#define Overview_Game_Types_hpp

#include "Engine/ModelTypes.hpp"

namespace cinek { namespace overview {

//////////////////////////////////////////////////////////////////////////

using GameObjectId = CommonModelId;
using EntityId = GameObjectId;


//////////////////////////////////////////////////////////////////////////

struct SimDebugMessage
{
    enum
    {
        kDrawLine,
        kDrawPoint
    }
    type;
    
    Point p0;
    Point p1;
    glm::vec3 color;
};

//////////////////////////////////////////////////////////////////////////

} /* namespace overview */ } /* namespace cinek */

#endif
