/**
 * @file    Core/EventBase.hpp
 * @author  Samir Sinha
 * @date    10/31/2014
 * @brief   std library allocator using custom allocation hooks
 * @copyright Copyright 2014 Samir Sinha.  All rights reserved.
 * @license ISC
 *          (http://www.isc.org/downloads/software-support-policy/isc-license/)
 */

#ifndef CK_Core_EventBase_hpp
#define CK_Core_EventBase_hpp

#include "cinek/types.hpp"

namespace cinek {

class EventBase
{
public:
    EventBase(uint32_t timeMs);

    virtual ~EventBase() {}
    virtual const UUID& classID() const = 0;

    uint32_t timestamp() const { return _ticks; }

private:
    uint32_t _ticks;
};

template<typename T> T* event_cast(const EventBase* evt) {
    return (evt->classID() == T::kClassID) ? static_cast<T*>(evt) : nullptr;
}

inline EventBase::EventBase(uint32_t ticks) :
    _ticks(ticks)
{
}

} /* namespace cinek */

#endif
