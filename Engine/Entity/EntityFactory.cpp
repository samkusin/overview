//
//  EntityFactory.cpp
//  Overview
//
//  Created by Samir Sinha on 5/21/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "EntityFactory.hpp"
#include "EntityStore.hpp"

#include "Comp/Transform.hpp"
#include "Comp/Renderable.hpp"
#include "Comp/Camera.hpp"
#include "Comp/MeshRenderable.hpp"

#include "Engine/MessagePublisher.hpp"
#include "Engine/Debug.hpp"
#include "Engine/Render/RenderTypes.hpp"
#include "CKGfx/MeshLibrary.hpp"
#include "CKGfx/TextureAtlas.hpp"

#include "entityevents.capnp.h"

#include <cinek/json/json.hpp>
#include <cstdio>

namespace cinek { namespace overview {

namespace component
{
    static void createRenderable
    (
        EntityStore& store,
        RenderResources& renderResources,
        Entity entity,
        const cinek::JsonValue& definitions,
        const cinek::JsonValue& data)
    {
        const char* typeName = data["type"].GetString();
        const char* dataName = data["name"].GetString();
        
        auto table = store.table<overview::component::Renderable>();
        auto meshTable = store.table<overview::component::MeshRenderable>();
        
        auto comp = table.addDataToEntity(entity);
        if (comp)
        {
            if (meshTable)
            {
                if (!strcmp(typeName, "mesh"))
                {
                    auto meshComp = meshTable.addDataToEntity(entity);
                    //  preload mesh
                    meshComp->meshHandle = renderResources.meshLibrary->load(dataName).data;
                
                    if (data.HasMember("bitmap"))
                    {
                        const char* bmpName = data["bitmap"].GetString();
                        meshComp->texHandle = renderResources.textureAtlas->loadTexture(bmpName).data;
                    }
                }
            }
        }
        else
        {
            OVENGINE_LOG_ERROR("createRenderable - "
                               "unable to allocate a Renderable for entity %" PRIu64 ".",
                               entity.id);
        }
    }
    
    static void destroyMeshRenderable
    (
        RenderResources& renderResources,
        Entity entity,
        component::MeshRenderable& component
    )
    {
        renderResources.meshLibrary->unload(component.meshHandle);
        renderResources.textureAtlas->unloadTexture(component.texHandle);
    }
    
    static void createCamera
    (
        EntityStore& store,
        Entity entity,
        const cinek::JsonValue& 
    )
    {
        auto table = store.table<overview::component::Camera>();
        auto comp = table.addDataToEntity(entity);
        if (!comp)
        {
            OVENGINE_LOG_ERROR("createRenderable - "
                               "unable to allocate a Renderable for entity %" PRIu64 ".",
                               entity.id);
        }

    }
    
}


Entity createEntity
(
    Entity::context_type context,
    EntityStore& store,
    RenderResources& renderResources,
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
            if (!strcmp(componentName, "renderable"))
            {
                component::createRenderable(store, renderResources,
                    entity,
                    definitions,
                    componentData);
            }
            else if (!strcmp(componentName, "transform"))
            {
                //  handled earlier
                continue;
            }
            else if (!strcmp(componentName, "camera"))
            {
                component::createCamera(store, entity, componentData);
            }
            
            //  allow customizations, extensions to the factory for custom
            //  components
            if (customCompFn)
            {
                customCompFn(entity, definitions, componentName, componentData);
            }
        }
    }
    
    /*
    if (eventPublisher)
    {
        capnp::MallocMessageBuilder message;
        msg::EntityCreatedEvent::Builder evt = message.initRoot<msg::EntityCreatedEvent>();
        evt.setEntityId(entity.id);
        eventPublisher->queue(message, msg::kEntityCreatedEvent);
    }
    */
    
    return entity;
}

void destroyEntityComponent
(
    Entity entity,
    component::EntityDataTable& dataTable,
    EntityStore& store,
    RenderResources& renderResources,
    const CustomComponentDestroyFn& customCompFn
)
{
    auto& rowset = dataTable.rowset();
    
    switch (dataTable.id())
    {
    case component::kMeshRenderable:
        {
            component::destroyMeshRenderable(renderResources, entity,
                *rowset.at<component::MeshRenderable>(dataTable.rowIndexFromEntity(entity))
            );
        }
        break;
    default:
        break;
    }
    if (customCompFn)
    {
        customCompFn(entity, dataTable.id());
    }
    
    dataTable.removeDataFromEntity(entity);
}


} /* namespace ovproto */ } /* namespace cinek */