//
//  MessageDispatcher.cpp
//  Overview
//
//  Created by Samir Sinha on 5/22/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "MessageDispatcher.hpp"
#include "MessageStream.hpp"
#include "MessagePublisher.hpp"

namespace cinek { namespace overview {

MessageStream setMessageStreamForPublisher
(
    MessageStream&& publishTargetStream,
    MessagePublisher& publisher
)
{
    auto acquiredStream = publisher.acquireStream(std::move(publishTargetStream));
    return acquiredStream;
}

void dispatchMessageStreamToPublisher
(
    MessageStream& stream,
    MessagePublisher& publisher
)
{
    publisher.dispatch(stream);
}

} /* namespace overview */ } /* namespace cinek */
