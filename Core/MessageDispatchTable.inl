
#include <algorithm>

namespace cinek {

template<typename MessageDataDerived>
void MessageDispatchTable::addEntry(
    MessageId id,
    const Delegate<MessageDataDerived>& delegate)
{
    _table.emplace_back(id, reinterpret_cast<const DelegateBase&>(delegate));
}

template<typename MessageDataDerived>
bool MessageDispatchTable::invoke(
    MessageId id,
    const Message& msg)
{
    auto it = lower_bound(id);
    if (it == _table.end() || it->first != id)
        return false;

    //auto& del = *reinterpret_cast<Delegate<MessageDataDerived>*>(&it->second);

    it->second(*static_cast<const MessageDataDerived*>(msg.data()));
    return true;
}

} /* namespace cinek */
