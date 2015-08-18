//
//  EntityFactory.cpp
//  Overview
//
//  Created by Samir Sinha on 5/21/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "EntityFactory.hpp"
#include "EntityStore.hpp"

#include "Transform.hpp"

#include "Engine/MessagePublisher.hpp"
#include "Engine/Debug.hpp"

#include <cinek/json/json.hpp>

namespace cinek { namespace overview {

Entity createEntity
(
    Entity::context_type context,
    EntityStore& store,
    const cinek::JsonValue& definitions,
    const char* name,
    const CustomComponentCreateFn& customCompFn
)
{
    Entity entity = store.create(context);
    
    //  generate an entity and its base components using a template
    if (name && name[0])
    {
        const cinek::JsonValue& templates = definitions["entity"];
   
        if (!templates.HasMember(name))
            return Entity::null();
    
        const cinek::JsonValue& templ = templates[name];
        if (!templ.HasMember("transform") ||
            (templ["transform"].IsBool() && templ["transform"].GetBool()))
        {
            store.table<component::Transform>().addDataToEntity(entity);
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

void destroyEntityComponent
(
    Entity entity,
    component::EntityDataTable& dataTable,
    EntityStore& store,
    const CustomComponentDestroyFn& customCompFn
)
{
    if (customCompFn)
    {
        customCompFn(entity, dataTable.id());
    }
    
    dataTable.removeDataFromEntity(entity);
}


} /* namespace ovproto */ } /* namespace cinek */