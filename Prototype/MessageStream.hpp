//
//  MessageStream.hpp
//  Overview
//
//  Created by Samir Sinha on 11/14/15.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overivew_Game_MessageStream_hpp
#define Overivew_Game_MessageStream_hpp

#include "MessageTypes.hpp"

#include <kj/io.h>
#include <capnp/message.h>

namespace cinek { namespace overview {

//  TODO: extract ring buffer code and isolate into its own class
class MessageStream : public kj::InputStream, public kj::OutputStream
{
    CK_CLASS_NON_COPYABLE(MessageStream);
    
public:
    MessageStream() = default;
    MessageStream(MessageBuffer& buffer);
    MessageStream(MessageStream&& stream);
    
    virtual size_t tryRead(void* buffer, size_t minBytes, size_t maxBytes) override;
    
    virtual void write(const void* buffer, size_t size) override;
    
    MessageBuffer& buffer() { return _buffer; }
    
private:
    MessageBuffer& _buffer;
};

MessageHeader& readMessageHeader(MessageStream& stream, MessageHeader& header);

void writeMessageHeader(MessageStream& stream, const MessageHeader& header);
void writeMessage(MessageStream& output, capnp::MessageBuilder& builder);

} /* namespace overview */ } /* namespace cinek */

#endif
