//
//  EntityService.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/11/15.
//
//

#include "EntityService.hpp"

#include "Engine/EntityUtility.hpp"
#include "Engine/Debug.hpp"

#include <ckentity/entitystore.hpp>
#include <cinek/filestreambuf.hpp>
#include <ckjson/json.hpp>
#include <ckjson/jsonstreambuf.hpp>

namespace cinek {
    namespace ove {

EntityService::EntityService(EntityUtility* context) :
    _context(context)
{
}

Entity EntityService::createEntity
(
    EntityContextType storeId,
    const char* ns,
    const char* templateName
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

void EntityService::addDefinitions
(
    const char* ns,
    const char* definitionsFilePath
)
{
    FileStreamBuf streamBuf(definitionsFilePath);
    
    if (!streamBuf) {
        OVENGINE_LOG_ERROR("EntityService.addDefinitions - "
                           "Failed to load %s", definitionsFilePath);
        return;
    }
    
    RapidJsonStdStreamBuf jsonStreamBuf(streamBuf);
    JsonDocument document(nullptr, 64*1024);
    if (document.ParseStream<0>(jsonStreamBuf).HasParseError()) {
        OVENGINE_LOG_ERROR("EntityService.addDefinitions - "
                           "Failed to parse %s", definitionsFilePath);
        return;
    }
    
    _context->addDefinitions(ns, document);
}

void EntityService::resetDefinitions()
{
    _context->resetDefinitions();
}

template<typename Component>
Component* EntityService::createData(Entity entity)
{
    auto table = _context->getStore(cinek_entity_context(entity)).table<Component>();
    return table.addDataForEntity(entity);
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
