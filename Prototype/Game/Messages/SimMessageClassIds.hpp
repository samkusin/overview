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
        const Message::ClassId kCreateEntity            = 0x00010000;
        const Message::ClassId kDestroyEntity           = 0x00010001;
    }

} /* namespace overview */ } /* namespace cinek */

#endif
