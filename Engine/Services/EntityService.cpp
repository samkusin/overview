//
//  EntityService.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/11/15.
//
//

#include "EntityService.hpp"
#include "Engine/EntityDatabase.hpp"
#include "Engine/Messages/Entity.hpp"

#include "Debug.hpp"

#include <ckentity/entitystore.hpp>
#include <cinek/filestreambuf.hpp>
#include <ckjson/json.hpp>
#include <ckjson/jsonstreambuf.hpp>
#include <ckmsg/client.hpp>

namespace cinek {
    namespace ove {

Entity EntityService::createEntity
(
    EntityContextType storeId,
    const std::string& ns,
    const std::string& templateName
)
{
    return _context->createEntity(storeId, ns, templateName);
}

template<typename Component>
component::Table<Component> EntityService::getTable(EntityContextType storeId)
{
    return _context->getStore(storeId).table<Component>();
}

EntityGroupTable EntityService::entityGroupTable
(
    EntityContextType storeId,
    EntityGroupMapId id
)
const
{
    return _context->getStore(storeId).entityGroupTable(id);
}

bool EntityService::isEntityValid(Entity e) const
{
    return _context->getStore(cinek_entity_context(e)).valid(e);
}

void EntityService::loadDefinitions
(
    const std::string& name,
    std::function<void(const EntityLoadDefinitionsResponse&)> cb
)
{
    EntityLoadDefinitionsRequest req;
    strncpy(req.name, name.c_str(), sizeof(req.name));
    _sender->client->send(_sender->server,
        kMsgEntityLoadDefinitions,
        makePayloadFromData(req),
        [cb](uint32_t, ckmsg::ClassId cid, const ckmsg::Payload* payload) {
            CK_ASSERT(cid==kMsgEntityLoadDefinitions);
            cb(*reinterpret_cast<const EntityLoadDefinitionsResponse*>(payload->data()));
        });
}

void EntityService::clearDefinitions(const std::string& path)
{
    _context->clearManifest(path);
}

template<typename Component>
Component* EntityService::createData(Entity entity)
{
    auto table = _context->getStore(cinek_entity_context(entity)).table<Component>();
    return table.addDataForEntity(entity);
}

bool EntityService::hasComponent(Entity entity, ComponentId compId) const
{
    auto table = _context->getStore(cinek_entity_context(entity)).entityTable(compId);
    if (!table)
        return false;
    return table->hasEntity(entity);
}

template<typename Component>
const Component* EntityService::getData(Entity entity) const
{
    auto table = _context->getStore(cinek_entity_context(entity)).table<Component>();
    return table.dataForEntity(entity);
}

template<typename Component>
Component* EntityService::getData(Entity entity)
{
    auto table = _context->getStore(cinek_entity_context(entity)).table<Component>();
    return table.dataForEntity(entity);
}


    } /* namespace ove */
} /* namespace cinek */
