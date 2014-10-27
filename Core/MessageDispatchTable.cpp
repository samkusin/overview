
#include "Core/MessageDispatchTable.hpp"

#include <algorithm>

namespace cinek {

MessageDispatchTable::MessageDispatchTable(
    size_t tableSize,
    const Allocator& allocator) :
    _table(allocator)
{
    _table.reserve(tableSize);
}

void MessageDispatchTable::sortEntries()
{
    std::sort(_table.begin(), _table.end(),
        [](const decltype(_table)::value_type& l, const decltype(_table)::value_type& r) -> bool
        {
            return l.first < r.first;
        });
}

void MessageDispatchTable::flushEntries()
{
    _table.clear();
}

auto MessageDispatchTable::lower_bound(MessageId id)
    -> decltype(_table)::iterator
{
    auto it = std::lower_bound(_table.begin(), _table.end(), id,
        [](const decltype(_table)::value_type& mapping, const MessageId& id) -> bool {
            return mapping.first < id;
        });
    return it;
}

} /* namespace cinek */

