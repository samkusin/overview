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
        const JsonValue& root = it->second->root();
        const JsonValue& entityDefinitions = root.FindMember("entity")->value;
        
        auto templateIt = entityDefinitions.FindMember(templateName.c_str());
        if (templateIt != entityDefinitions.MemberEnd()) {
            auto& store = getStore(context);
            entity = store.create(context);
            
            //  create renderable component first if it exists. useful so that
            //  other components that reply on renderable have the data they
            //  need
            const cinek::JsonValue& templ = templateIt->value;
            cinek::JsonValue::ConstMemberIterator it = templ.FindMember("renderable");
            if (it != templ.MemberEnd()) {
                 _factory->onCustomComponentCreateFn(entity,
                        store, it->name.GetString(),
                        root,
                        it->value);
            }
        
            for (it = templ.MemberBegin();
                 it != templ.MemberEnd();
                 ++it)
            {
                const char* componentName = it->name.GetString();
                
                if (!strcasecmp(componentName, "renderable")) {
                    continue;   // handled before this loop
                }
                
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

bool EntityDatabase::isValid(Entity entity) const
{
    return getStore(cinek_entity_context(entity)).valid(entity);
}

Entity EntityDatabase::cloneEntity
(
    EntityContextType context,
    Entity source
)
{
    Entity cloned = getStore(context).create(context);
    _factory->onCustomComponentEntityCloneFn(cloned, source);
    return cloned;
}

void EntityDatabase::destroyEntity(Entity entity)
{
    getStore(cinek_entity_context(entity)).destroy(entity);
    
    //  TODO - consider delaying this until the gc() phase
    //  need a way to flag an entity as dead and group them together so
    //  that the gc() phase only acts on destroyed entities for that timeslice
    //
    _factory->onCustomComponentEntityDestroyFn(entity);
}

void EntityDatabase::gc()
{
    for (auto& store : _stores) {
        store.gc();
    }
}

void EntityDatabase::setManifest
(
    std::string name,
    std::shared_ptr<AssetManifest> manifest
)
{
    _manifests.emplace(std::move(name), std::move(manifest));
}

void EntityDatabase::clearManifest(const std::string& name)
{
    _manifests.erase(name);
}

const AssetManifest* EntityDatabase::getManifest
(
    const std::string& name
)
const
{
    auto it = _manifests.find(name);
    if (it == _manifests.end())
        return nullptr;
    
    return it->second.get();
}


    }   /* namespace ove */
}   /* namespace cinek */
