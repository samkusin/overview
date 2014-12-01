//
//  SimMessageClassIds.hpp
//  Overview
//
//  Created by Samir Sinha on 11/26/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_SimMessageClassIds_hpp
#define Overview_Game_SimMessageClassIds_hpp

#include "Core/Message.hpp"

namespace cinek { namespace overview {

namespace SimCommand
{
    constexpr auto kCreateEntity         = Message::createClassId(Message::kCommandMessage, 0x01, 0x0000);
    constexpr auto kDestroyEntity        = Message::createClassId(Message::kCommandMessage, 0x01, 0x0001);
    constexpr auto kMoveEntity           = Message::createClassId(Message::kCommandMessage, 0x01, 0x0002);
}

} /* namespace overview */ } /* namespace cinek */

#endif
