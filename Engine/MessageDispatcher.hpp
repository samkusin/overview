//
//  MessageDispatcher.hpp
//  Overview
//
//  Created by Samir Sinha on 5/22/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Message_Dispatcher_hpp
#define Overview_Message_Dispatcher_hpp

#include "EngineTypes.hpp"
#include "MessageStream.hpp"
#include "MessagePublisher.hpp"

namespace cinek { namespace overview {


template<typename Publisher>
MessageStream setMessageStreamForPublisher
(
    MessageStream&& publishTargetStream,
    Publisher& publisher
)
{
    auto acquiredStream = publisher.acquireStream(std::move(publishTargetStream));
    return acquiredStream;
}

template<typename Publisher>
void dispatchMessageStreamToPublisher
(
    MessageStream& stream,
    Publisher& publisher
)
{
    publisher.dispatch(stream);
}



} /* namespace overview */ } /* namespace cinek */


#endif /* ifdef Overview_Message_Dispatcher_hpp */

