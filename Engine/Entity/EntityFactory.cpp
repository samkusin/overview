//
//  EntityFactory.cpp
//  Overview
//
//  Created by Samir Sinha on 5/21/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "EntityFactory.hpp"
#include "EntityDatabase.hpp"

#include "Comp/Renderable.hpp"

#include "Engine/MessagePublisher.hpp"
#include "Engine/Debug.hpp"
#include "Engine/Render/RenderScene.hpp"
#include "CKGfx/MeshLibrary.hpp"
#include "CKGfx/TextureAtlas.hpp"

#include "entityevents.capnp.h"

#include <cinek/json/json.hpp>

namespace cinek { namespace overview {

namespace component
{
    static void createRenderable
    (
        EntityDatabase& db,
        RenderResources& renderResources,
        EntityObject& object,
        const cinek::JsonValue& definitions,
        const cinek::JsonValue& data)
    {
        const char* typeName = data["type"].GetString();
        const char* dataName = data["name"].GetString();
        
        auto comp = db.addComponentToEntity<overview::component::Renderable>(object.id());
        if (comp)
        {
            comp->viewFilterMask = parseFlagsToUint(
                definitions["renderable_filter_flags"],
                data["filter"].GetString()
            );

            if (!strcmp(typeName, "mesh"))
            {
                //  preload mesh
                comp->meshHandle = renderResources.meshLibrary->load(dataName).data;
            }
            
            if (data.HasMember("bitmap"))
            {
                const char* bmpName = data["bitmap"].GetString();
                comp->texHandle = renderResources.textureAtlas->loadTexture(bmpName).data;
            }
        }
        else
        {
            OVENGINE_LOG_ERROR("createRenderable - "
                               "unable to allocate a Renderable for entity %" PRIu64 ".",
                               object.id());
        }
    }
}


EntityObject* createEntity
(
    EntityDatabase& db,
    RenderResources& renderResources,
    const cinek::JsonValue& definitions,
    const char* name,
    MessagePublisher* eventPublisher,
    const CustomComponentCreateFn& customCompFn
)
{
    EntityObject* entity = db.createEntity();
    
    //  generate an entity and its base components using a template
    if (name && name[0])
    {
        const cinek::JsonValue& templates = definitions["entity"];
   
        if (!templates.HasMember(name))
            return nullptr;
    
        const cinek::JsonValue& templ = templates[name];
        for (cinek::JsonValue::ConstMemberIterator it = templ.MemberBegin();
             it != templ.MemberEnd();
             ++it)
        {
            const char* componentName = it->name.GetString();
            const cinek::JsonValue& componentData = it->value;
            
            //  renderable
            //      load resources based on type
            if (!strcmp(componentName, "renderable"))
            {
                component::createRenderable(db, renderResources,
                    *entity,
                    definitions,
                    componentData);
            }
            
            //  allow customizations, extensions to the factory for custom
            //  components
            if (customCompFn)
            {
                customCompFn(*entity, definitions, componentData);
            }
        }
    }
    
    if (eventPublisher)
    {
        capnp::MallocMessageBuilder message;
        msg::EntityCreatedEvent::Builder evt = message.initRoot<msg::EntityCreatedEvent>();
        evt.setEntityId(entity->id());
        eventPublisher->queue(message, msg::kEntityCreatedEvent);
    }
    
    return entity;
}


} /* namespace ovproto */ } /* namespace cinek */