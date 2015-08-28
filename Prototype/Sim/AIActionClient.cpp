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
    Action& action,
    double time
)
{
    return true;
}

bool AIActionClient::update
(
    Action& action,
    double time,
    double dt
)
{
    return true;
}

void AIActionClient::abort
(
    Action& action,
    double time
)
{
}

Action::return_type AIActionClient::end
(
    Action& action,
    double time
)
{
    return 0;
}

} /* namespace ovproto */ } /* namespace cinek */
