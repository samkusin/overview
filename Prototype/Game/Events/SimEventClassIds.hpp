//
//  SimEventClassIds.h
//  Overview
//
//  Created by Samir Sinha on 11/28/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_SimEventClassIds_hpp
#define Overview_Game_SimEventClassIds_hpp

namespace cinek { namespace overview {

namespace SimEvent
{
    constexpr auto kCreateEntity            = Message::createClassId(Message::kEventMessage, 0x01, 0x0000);
    constexpr auto kDestroyEntity           = Message::createClassId(Message::kEventMessage, 0x01, 0x0001);
    constexpr auto kMoveEntity              = Message::createClassId(Message::kEventMessage, 0x01, 0x0002);
}

} /* namespace overview */ } /* namespace cinek */


#endif
