//
//  EntityService.cpp
//  Overview
//
//  Created by Samir Sinha on 7/24/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "EntityService.hpp"
#include "Engine/Entity/EntityFactory.hpp"

#include <cinek/json/json.hpp>

namespace cinek {
    namespace ovproto {


EntityService::EntityService(AppContext& context) :
    _context(context._o)
{
}

Entity EntityService::create(Entity::context_type context)
{
    return _context->entityStore->create(context);
}
    
Entity EntityService::create(const char* templateName, Entity::context_type context)
{
    //  TODO - optimize?
    auto it = _context->documentMap->find(kDocumentId_EntityTemplates);
    if (it == _context->documentMap->end())
        return Entity::null();
    
    return overview::createEntity(context, *_context->entityStore,
        *it->second.get(),
        templateName,
        _context->createComponentCb);
}
    
void EntityService::destroy(Entity entity)
{
    _context->entityStore->destroy(entity);
}

void EntityService::setCreateComponentCallback
(
    const overview::CustomComponentCreateFn& cb
)
{
    _context->createComponentCb = cb;
}

void EntityService::setDestroyComponentCallback
(
    const overview::CustomComponentDestroyFn& cb
)
{
    _context->destroyComponentCb = cb;
}
    
    } /* namespace ovproto */
} /* namespace cinek */