/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Cinekine Media
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @file    Core/Message.hpp
 * @author  Samir Sinha
 * @date    10/20/2014
 * @brief   Queued request and result dispatch to application supplied callbacks
 * @copyright Cinekine
 */

#ifndef CK_Core_Message_hpp
#define CK_Core_Message_hpp

#include "Core/SDO/Base.hpp"

namespace cinek {

struct MessageId
{
    using ClassId           = uint32_t;
    using InstanceId        = uint32_t;

    ClassId classId;
    InstanceId instanceId;
};

inline bool operator<(const MessageId& l, const MessageId& r);
inline bool operator!=(const MessageId& l, const MessageId& r);

class Message
{
public:
    using DataType          = sdo::Base;

    Message() = default;
    Message(MessageId id);

    MessageId id() const { return _id; }

    const DataType* data() const { return _data; }
    DataType* data() { return _data; }

    void setData(DataType* data) { _data = data; }
    DataType* clearData() { auto data = _data; _data = nullptr; return data; }

private:
    MessageId _id;
    DataType* _data;
};

} /* namespace cinek */

#include "Core/Message.inl"

#endif
