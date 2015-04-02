//
//  SimulationContext.hpp
//  Overview
//
//  Created by Samir Sinha on 11/28/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_SimulationContext_hpp
#define Overview_Game_SimulationContext_hpp

#include "Shared/GameTypes.hpp"
#include "Game/Messages/CommandResponse.hpp"
#include "Core/MessageQueue.hpp"
#include "cinek/taskscheduler.hpp"

namespace cinek { namespace overview {

struct SimulationContext
{
    Allocator allocator;
    MessageQueue* resultMsgQueue;
    MessageQueue* eventQueue;
    TaskScheduler* scheduler;
    Message::SequenceId sequenceId;
};

} /* namespace overview */ } /* namespace cinek */


#endif
