//
//  EntityFactory.cpp
//  Overview
//
//  Created by Samir Sinha on 5/21/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "EntityUtility.hpp"
#include "EntityStoreDictionary.hpp"

#include "Transform.hpp"

#include "Engine/Debug.hpp"

#include <ckentity/entitystore.hpp>
#include <ckjson/json.hpp>

namespace cinek { namespace ove {

Entity createEntity
(
    EntityContextType context,
    EntityStore& store,
    const cinek::JsonValue& definitions,
    const char* name,
    const CustomComponentCreateFn& customCompFn
)
{
    Entity entity = 0;
    
    //  generate an entity and its base components using a template
    if (name && name[0])
    {
        const cinek::JsonValue& templates = definitions["entity"];
   
        if (!templates.HasMember(name))
            return 0;
    
        entity = store.create(context);
    
        const cinek::JsonValue& templ = templates[name];
        if (!templ.HasMember("transform") ||
            (templ["transform"].IsBool() && templ["transform"].GetBool()))
        {
            store.table<ove::TransformComponent>().addDataToEntity(entity);
        }
        for (cinek::JsonValue::ConstMemberIterator it = templ.MemberBegin();
             it != templ.MemberEnd();
             ++it)
        {
            const char* componentName = it->name.GetString();
            const cinek::JsonValue& componentData = it->value;
            
            //  renderable
            //      load resources based on type
            if (!strcmp(componentName, "transform"))
            {
                //  handled earlier
                continue;
            }
            
            //  allow customizations, extensions to the factory for custom
            //  components
            if (customCompFn)
            {
                customCompFn(entity, definitions, componentName, componentData);
            }
        }
    }
    
    return entity;
}


EntityUtility::EntityUtility
(
    EntityStoreDictionary& entityStoreDict,
    CustomComponentCreateFn customCreateFn
) :
    _entityStoreDictionary(&entityStoreDict),
    _customCreateFn(std::move(customCreateFn)),
    _entityDefinitions(nullptr, 64*1024)    // TODO, make configurable?
{
}

Entity EntityUtility::createEntity
(
    EntityContextType context,
    const char* templateNs,
    const char* templateName
)
{
    auto it = _entityDefinitions.FindMember(templateNs);
    if (it == _entityDefinitions.MemberEnd())
        return 0;
    
    return cinek::ove::createEntity(context,
        _entityStoreDictionary->getStore(context),
        it->value,
        templateName,
        _customCreateFn);
    
}

void EntityUtility::destroyEntity(Entity entity)
{
    //  component destruction invoked during EntityStore garbage collection
    _entityStoreDictionary->getStore(cinek_entity_context(entity)).destroy(entity);
}

const EntityStore& EntityUtility::getStore(EntityContextType context) const
{
    return _entityStoreDictionary->getStore(context);
}

EntityStore& EntityUtility::getStore(EntityContextType context)
{
    return const_cast<EntityStore&>(
        static_cast<const EntityUtility*>(this)->getStore()
    );
}
    
void EntityUtility::addDefinitions(const char* ns, const JsonValue& definitions)
{
    auto definitionNsIt = _entityDefinitions.FindMember(ns);
    if (definitionNsIt == _entityDefinitions.MemberEnd()) {
        JsonValue definitionNs(json::kObjectType);
        definitionNs.CopyFrom(definitions, _entityDefinitions.GetAllocator());
        JsonValue nsKey(json::kStringType);
        nsKey.SetString(ns, _entityDefinitions.GetAllocator());
        _entityDefinitions.AddMember(nsKey, definitionNs, _entityDefinitions.GetAllocator());
    }
    else {
        OVENGINE_LOG_WARN("EntityUtility.addDefinitions - Namespace '%s' already exists.  "
                          "Ignoring...",
                          ns);
    }
}

void EntityUtility::resetDefinitions()
{
    _entityDefinitions.~JsonDocument();
    ::new(&_entityDefinitions) JsonDocument(nullptr, 64*1024);
}
  

} /* namespace ove */ } /* namespace cinek */