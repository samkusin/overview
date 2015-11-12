//
//  EntityStoreDictionary.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/11/15.
//
//

#include "EntityStoreDictionary.hpp"

#include <cinek/debug.h>

namespace cinek {
    namespace ove {

EntityStoreDictionary::EntityStoreDictionary
(
    const std::vector<EntityStore::InitParams>& stores
)
{
    auto dictSz = std::min((uint32_t)stores.size(), 1U << kCKEntityContextBits);
    dictSz = std::min(dictSz, 256U);
    if (dictSz > 0) {
        _stores.reserve(dictSz);
        
        for (auto& store : stores) {
            _stores.emplace_back(store);
        }
    }
}
    
EntityStoreDictionary::EntityStoreDictionary(EntityStoreDictionary&& other) :
    _stores(std::move(other._stores))
{
}

EntityStoreDictionary& EntityStoreDictionary::operator=(EntityStoreDictionary&& other)
{
    _stores = std::move(other._stores);
    return *this;
}

const EntityStore& EntityStoreDictionary::getStore(EntityContextType index) const
{
    if (index >= _stores.size())
        index = 0;
    CK_ASSERT(index < _stores.size());
    return _stores[index];
}

EntityStore& EntityStoreDictionary::getStore(EntityContextType index)
{
    return const_cast<EntityStore&>(
        static_cast<const EntityStoreDictionary*>(this)->getStore(index)
    );
}

void EntityStoreDictionary::gc()
{
    for (auto& store : _stores) {
        store.gc();
    }
}


    }   /* namespace ove */
}   /* namespace cinek */
