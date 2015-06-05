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

class MessageStream : public kj::InputStream, public kj::OutputStream
{
    CK_CLASS_NON_COPYABLE(MessageStream);
    
public:
    MessageStream() = default;
    MessageStream(size_t bufferSize, const Allocator& allocator=Allocator());
    MessageStream(MessageStream&& stream);
    
    MessageStream& operator=(MessageStream&& other);
    
    virtual size_t tryRead(void* buffer, size_t minBytes, size_t maxBytes) override;
    
    virtual void write(const void* buffer, size_t size) override;
    
    void reset();
    
   // Buffer<Allocator>& buffer() { return _buffer; }
    
private:
    Buffer<Allocator> _buffer;
};

MessageHeader& readMessageHeader(MessageStream& stream, MessageHeader& header);

void writeMessageHeader(MessageStream& stream, const MessageHeader& header);
void writeMessage(MessageStream& output, capnp::MessageBuilder& builder);

} /* namespace overview */ } /* namespace cinek */

#endif
