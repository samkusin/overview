//
//  MessageStream.cpp
//  Overview
//
//  Created by Samir Sinha on 4/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "MessageStream.hpp"

#include <cinek/debug.hpp>
#include <capnp/serialize.h>

namespace cinek { namespace overview {

MessageStream::MessageStream(MessageBuffer& buffer) :
    _buffer(buffer)
{
}

MessageStream::MessageStream(MessageStream&& stream) :
    _buffer(stream._buffer)
{
}

size_t MessageStream::tryRead(void* buffer, size_t minBytes, size_t maxBytes)
{
    return _buffer.read(reinterpret_cast<uint8_t*>(buffer), maxBytes);
}

    
void MessageStream::write(const void* buf, size_t size)
{
    CK_ASSERT(size <= _buffer.writeAvailable());
    _buffer.write(reinterpret_cast<const uint8_t*>(buf), size);
}

void writeMessage(MessageStream& output, ::capnp::MessageBuilder& builder)
{
    capnp::writeMessage(output, builder);
}

MessageHeader& readMessageHeader(MessageStream& stream, MessageHeader& header)
{
    uint16_t s;
    auto cnt = stream.tryRead(&s, sizeof(s), sizeof(s));
    if (!cnt)
    {
        header.type = MessageHeader::kEnd;
        return header;
    }
    else if (cnt < sizeof(s))
    {
        header.type = MessageHeader::kInvalid;
        return header;
    }
    
    stream.read((void*)&s, sizeof(s));
    s = ntohs(s);
    if (s > MessageHeader::kResponse)
    {
        header.type = MessageHeader::kInvalid;
    }
    else
    {
        header.type = static_cast<MessageHeader::Type>(ntohs(s));
    }
    if (header.type != MessageHeader::kInvalid)
    {
        auto cnt = stream.tryRead(header.classId.bytes,
            sizeof(header.classId.bytes),
            sizeof(header.classId.bytes));
        if ( cnt < sizeof(header.classId.bytes) )
            header.type = MessageHeader::kInvalid;
    }
    if (header.type != MessageHeader::kInvalid)
    {
        uint32_t l;
        stream.read(&l, sizeof(l));
        header.seqId = ntohl(l);
    }
        
    return header;
}

void writeMessageHeader(MessageStream& stream, const MessageHeader& header)
{
    auto ns = htons(static_cast<int16_t>(header.type));
    stream.write(&ns, sizeof(ns));
    stream.write(header.classId.bytes, sizeof(header.classId.bytes));
    auto l = htonl(header.seqId);
    stream.write(&l, sizeof(l));
}


} /* namespace overview */ } /* namespace cinek */