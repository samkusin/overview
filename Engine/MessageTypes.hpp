//
//  MessageTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 4/20/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_MessageTypes_hpp
#define Overview_MessageTypes_hpp

#include <cinek/buffer.hpp>
#include <cinek/allocator.hpp>

namespace cinek { namespace overview {

using MessageDelegateHandle = uint32_t;
using MessageBuffer = Buffer<Allocator>;

class MessageStream;

template<typename Delegate> class MessagePublisher;

struct MessageHeader
{
    enum Type
    {
        kEnd        = 0,
        kEvent      = 1,
        kCommand    = 2,
        kResponse   = 3,
        kInvalid    = -1
    };

    using ClassId = UUID;
    using SequenceId = uint32_t;
    
    Type type = kInvalid;
    ClassId classId;
    SequenceId seqId = 0;
};

//  The Message Namespace
namespace msg {}

} /* namespace overview */ } /* namespace cinek */


#endif
