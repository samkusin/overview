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
 * @file    Core/MessageDispatchTable.hpp
 * @author  Samir Sinha
 * @date    10/20/2014
 * @brief   Queued request and result dispatch to application supplied callbacks
 * @copyright Cinekine
 */

#ifndef CK_Core_MessageDispatchTable_hpp
#define CK_Core_MessageDispatchTable_hpp

#include "Core/Message.hpp"

#include "cinek/allocator.hpp"
#include "cinek/vector.hpp"

#include <functional>

namespace cinek {

class MessageDispatchTable
{
public:
    template<typename MessageData>
    using Delegate = std::function<void(const MessageData&)>;

    MessageDispatchTable(size_t tableSize,
                         const Allocator& allocator=Allocator());

    template<typename MessageDataDerived>
    void addEntry(MessageId id, const Delegate<MessageDataDerived>& delegate);
    void sortEntries();
    void flushEntries();


    template<typename MessageDataDerived>
    bool invoke(MessageId id, const Message& msg);

private:
    using DelegateBase = Delegate<Message::DataType>;
    vector<std::pair<MessageId, DelegateBase>> _table;
    decltype(_table)::iterator lower_bound(MessageId id);
};

} /* namespace cinek */

#include "Core/MessageDispatchTable.inl"

#endif