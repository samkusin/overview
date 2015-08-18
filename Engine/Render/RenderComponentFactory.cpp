//
//  RenderComponentFactory.hpp
//  Overview
//
//  Created by Samir Sinha on 8/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "RenderComponentFactory.hpp"

#include "Engine/Entity/EntityStore.hpp"
#include "Engine/Render/Renderable.hpp"
#include "Engine/Render/Camera.hpp"
#include "Engine/Render/MeshRenderable.hpp"
#include "Engine/Entity/EntityDataTable.hpp"
#include "Engine/Debug.hpp"

#include "CKGfx/MeshLibrary.hpp"
#include "CKGfx/TextureAtlas.hpp"
#include "CKGfx/ShaderLibrary.hpp"

#include <cinek/json/json.hpp>

namespace cinek {
    namespace overview {
    
namespace component
{

static bool createRenderable
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
        return false;
    }
    
    return true;
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

static bool createCamera
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
        return false;
    }
    return true;
}


ComponentFactoryResult createRenderable
(
    Entity entity,
    const cinek::JsonValue& definitions,
    const char* componentName,
    const cinek::JsonValue& data,
    EntityStore& store,
    RenderResources& resources
)
{
    if (strncmp(componentName, "renderable", sizeof("renderable")) != 0)
        return ComponentFactoryResult::kPassthrough;
    
    if (!createRenderable(store, resources, entity, definitions, data))
        return ComponentFactoryResult::kFailed;
    
    return ComponentFactoryResult::kSuccess;
}

ComponentFactoryResult destroyRenderable
(
    Entity entity,
    ComponentId compId,
    EntityStore& store,
    RenderResources& renderResources
)
{
    if (compId == component::kMeshRenderable)
    {
        auto comp = store.table<component::MeshRenderable>().dataForEntity(entity);
        if (comp)
        {
            destroyMeshRenderable(renderResources, entity, *comp);
        }
        return ComponentFactoryResult::kSuccess;
    }
    
    return ComponentFactoryResult::kPassthrough;
}


ComponentFactoryResult createCamera
(
    Entity entity,
    const cinek::JsonValue& definitions,
    const char* componentName,
    const cinek::JsonValue& data,
    EntityStore& store
)
{
    if (strncmp(componentName, "camera", sizeof("camera")) != 0)
        return ComponentFactoryResult::kPassthrough;
    
    if (!component::createCamera(store, entity, data))
        return ComponentFactoryResult::kFailed;
    
    return ComponentFactoryResult::kSuccess;
}

ComponentFactoryResult destroyCamera
(
    Entity entity,
    ComponentId compId,
    EntityStore& store
)
{
    if (compId != component::kCamera)
        return ComponentFactoryResult::kPassthrough;
    
    return ComponentFactoryResult::kSuccess;
}

} /* namespace component */
    
    } /* namespace overview */
} /* namespace cinek */
