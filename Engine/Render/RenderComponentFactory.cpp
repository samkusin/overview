//
//  RenderComponentFactory.hpp
//  Overview
//
//  Created by Samir Sinha on 8/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "RenderComponentFactory.hpp"

#include "Engine/Render/Comp/Renderable.hpp"
#include "Engine/Render/Comp/Camera.hpp"
#include "Engine/Render/Comp/MeshRenderable.hpp"
#include "Engine/Debug.hpp"

#include "CKGfx/MeshLibrary.hpp"
#include "CKGfx/TextureAtlas.hpp"
#include "CKGfx/ShaderLibrary.hpp"

#include <ckentity/entitydatatable.hpp>
#include <ckentity/entitystore.hpp>
#include <ckjson/json.hpp>

namespace cinek {
    namespace overview {
    
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
    
    auto table = store.table<RenderableComponent>();
    auto meshTable = store.table<MeshRenderableComponent>();
    
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
                           entity);
        return false;
    }
    
    return true;
}


static bool createCamera
(
    EntityStore& store,
    Entity entity,
    const cinek::JsonValue& 
)
{
    auto table = store.table<CameraComponent>();
    auto comp = table.addDataToEntity(entity);
    if (!comp)
    {
        OVENGINE_LOG_ERROR("createCamera - "
                           "unable to allocate a Renderable for entity %" PRIu64 ".",
                           entity);
        return false;
    }
    return true;
}


ComponentFactoryResult createRenderableComponent
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


static ComponentFactoryResult destroyMeshRenderable
(
    EntityDataTable& entityTable,
    ComponentRowIndex compRowIndex,
    RenderResources& renderResources
)
{
    if (entityTable.id() != kMeshRenderableComponent)
        return ComponentFactoryResult::kPassthrough;
    
    auto component = entityTable.rowset().at<MeshRenderableComponent>(compRowIndex);
    if (component)
    {
        renderResources.meshLibrary->unload(component->meshHandle);
        renderResources.textureAtlas->unloadTexture(component->texHandle);
    }
    
    return ComponentFactoryResult::kSuccess;
}


ComponentFactoryResult destroyRenderableComponent
(
    EntityDataTable& entityTable,
    ComponentRowIndex compRowIndex,
    RenderResources& renderResources
)
{
    if (entityTable.id() == kMeshRenderableComponent)
        return destroyMeshRenderable(entityTable, compRowIndex, renderResources);
    
    if (entityTable.id() != kRenderableComponent)
        return ComponentFactoryResult::kPassthrough;

    return ComponentFactoryResult::kSuccess;
}


ComponentFactoryResult createCameraComponent
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
    
    if (!createCamera(store, entity, data))
        return ComponentFactoryResult::kFailed;
    
    return ComponentFactoryResult::kSuccess;
}

ComponentFactoryResult destroyCameraComponent
(
    EntityDataTable& entityTable,
    ComponentRowIndex compRowIndex
)
{
    if (entityTable.id() != kCameraComponent)
        return ComponentFactoryResult::kPassthrough;
    
    return ComponentFactoryResult::kSuccess;
}
    
    } /* namespace overview */
} /* namespace cinek */
