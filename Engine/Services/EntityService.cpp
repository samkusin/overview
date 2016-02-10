//
//  EntityService.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/11/15.
//
//

#include "EntityService.hpp"
#include "Engine/EntityDatabase.hpp"

#include "Debug.hpp"

#include <ckentity/entitystore.hpp>
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

void EntityService::destroyEntity(Entity entity)
{
    _context->destroyEntity(entity);
}

Entity EntityService::cloneEntity(EntityContextType context, Entity entity)
{
    return _context->cloneEntity(context, entity);
}

bool EntityService::isEntityValid(Entity e) const
{
    return _context->getStore(cinek_entity_context(e)).valid(e);
}

void EntityService::addDefintions
(
    std::string name,
    std::shared_ptr<AssetManifest> manifest
)
{
    _context->setManifest(std::move(name), std::move(manifest));
}

void EntityService::clearDefinitions(const std::string& path)
{
    _context->clearManifest(path);
}

const AssetManifest* EntityService::getDefinitions(const std::string& name) const
{
    return _context->getManifest(name);
}


    } /* namespace ove */
} /* namespace cinek */
