//
//  AIActionClient.cpp
//  Overview
//
//  Created by Samir Sinha on 8/14/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "AIActionClient.hpp"


namespace cinek { namespace ovproto {

bool AIActionClient::begin
(
    AIAction& action,
    double time
)
{
    return true;
}

bool AIActionClient::update
(
    AIAction& action,
    double time,
    double dt
)
{
    return true;
}

AIAction::return_type AIActionClient::end
(
    AIAction& action,
    double time
)
{
    return 0;
}

} /* namespace ovproto */ } /* namespace cinek */
