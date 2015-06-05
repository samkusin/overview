//
//  Stage.hpp
//  Overview
//
//  Created by Samir Sinha on 4/15/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Stage_hpp
#define Overview_App_Stage_hpp

#include "GameTypes.hpp"
#include "Engine/EngineContext.hpp"

namespace cinek { namespace ovproto {

class Stage 
{
public:
    virtual ~Stage() {}
    
    virtual void update(overview::EngineContext& context) = 0;
};


} /* namespace ovproto */ } /* namespace cinek */


#endif /* ifdef Overview_App_Stage_hpp */
