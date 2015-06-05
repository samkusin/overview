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

namespace cinek { namespace overview {

class MessageStream;
class MessagePublisher;

MessageStream setMessageStreamForPublisher
(
    MessageStream&& publishTargetStream,
    MessagePublisher& publisher
);

void dispatchMessageStreamToPublisher
(
    MessageStream& stream,
    MessagePublisher& publisher
);

} /* namespace overview */ } /* namespace cinek */


#endif /* ifdef Overview_Message_Dispatcher_hpp */

