//
//  EntityDatabase.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/11/15.
//
//

#include "EntityDatabase.hpp"
#include "AssetManifest.hpp"
#include "Debug.hpp"

namespace cinek {
    namespace ove {

EntityDatabase::EntityDatabase
(
    const std::vector<EntityStore::InitParams>& stores,
    EntityComponentFactory& factory
) :
    _factory(&factory)
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
    
EntityDatabase::EntityDatabase(EntityDatabase&& other) :
    _stores(std::move(other._stores)),
    _manifests(std::move(other._manifests)),
    _factory(other._factory)
{
    other._factory = nullptr;
}

EntityDatabase& EntityDatabase::operator=(EntityDatabase&& other)
{
    _stores = std::move(other._stores);
    _manifests = std::move(other._manifests);
    _factory = other._factory;
    other._factory = nullptr;
    return *this;
}

const EntityStore& EntityDatabase::getStore(EntityContextType index) const
{
    if (index >= _stores.size())
        index = 0;
    CK_ASSERT(index < _stores.size());
    return _stores[index];
}

EntityStore& EntityDatabase::getStore(EntityContextType index)
{
    return const_cast<EntityStore&>(
        static_cast<const EntityDatabase*>(this)->getStore(index)
    );
}

Entity EntityDatabase::createEntity
(
    EntityContextType context,
    const std::string& ns,
    const std::string& templateName
)
{
    Entity entity = 0;
    auto it = _manifests.find(ns);
    if (it != _manifests.end()) {
        const JsonValue& root = it->second->root("entity");
        auto templateIt = root.FindMember(templateName.c_str());
        if (templateIt != root.MemberEnd()) {
            auto& store = getStore(context);
            entity = store.create(context);
        
            const cinek::JsonValue& templ = templateIt->value;
            for (cinek::JsonValue::ConstMemberIterator it = templ.MemberBegin();
                 it != templ.MemberEnd();
                 ++it)
            {
                const char* componentName = it->name.GetString();
                const cinek::JsonValue& componentData = it->value;
                
                _factory->onCustomComponentCreateFn(entity,
                        store, componentName,
                        root,
                        componentData);
            }
        }
    }
    
    return entity;
    
}

void EntityDatabase::destroyEntity(Entity entity)
{
    getStore(cinek_entity_context(entity)).destroy(entity);
}

void EntityDatabase::gc()
{
    for (auto& store : _stores) {
        store.gc([this](EntityDataTable& table, ComponentRowIndex compRowIndex) {
            _factory->onCustomComponentDestroyFn(table, compRowIndex);
        });
    }
}

void EntityDatabase::setManifest
(
    unique_ptr<AssetManifest>&& manifest
)
{
    _manifests.emplace(manifest->name(), std::move(manifest));
}

void EntityDatabase::clearManifest(const std::string& name)
{
    _manifests.erase(name);
}


    }   /* namespace ove */
}   /* namespace cinek */
