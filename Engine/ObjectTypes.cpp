//
//  ObjectTypes.cpp
//  Overview
//
//  Created by Samir Sinha on 5/22/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "ObjectTypes.hpp"

namespace cinek { namespace overview {

namespace msg
{
    UUID kCreateEntityRequest = {
        { 0x74, 0xec, 0x68, 0x0a, 0x3a, 0x7d, 0x41, 0x50, 0xa5, 0x28, 0xf6, 0xd5, 0x1f, 0x72, 0x5f, 0xeb }
    };
    UUID kEntityCreatedEvent = {
        { 0x70, 0xc2, 0xa8, 0x41, 0x95, 0x3c, 0x4d, 0xca, 0x8a, 0x7d, 0x41, 0x42, 0x51, 0xbc, 0x71, 0xbe }
    };
    UUID kEntityMovedEvent = {
        { 0x33, 0xf9, 0xb4, 0xc6, 0x2d, 0x72, 0x43, 0x72, 0x88, 0xe8, 0x4b, 0xf3, 0x2c, 0xc2, 0x0a, 0xe8 }
    };
}


} /* namespace overview */ } /* namespace cinek */