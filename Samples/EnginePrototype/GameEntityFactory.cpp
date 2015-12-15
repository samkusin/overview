//
//  GamneEntityFactory.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/10/15.
//
//

#include "GameEntityFactory.hpp"
#include "Engine/Scenes/Scene.hpp"
#include "Engine/Scenes/SceneDataContext.hpp"
#include "Engine/Render/RenderGraph.hpp"
#include "CKGfx/Context.hpp"

#include <ckjson/json.hpp>

namespace cinek {

GameEntityFactory::GameEntityFactory
(
    gfx::Context& gfxContext,
    ove::SceneDataContext& sceneData,
    ove::Scene& scene,
    ove::RenderGraph& renderGraph
) :
    _gfxContext(&gfxContext),
    _sceneDataContext(&sceneData),
    _scene(&scene),
    _renderGraph(&renderGraph)
{
}
                      

void GameEntityFactory::onCustomComponentCreateFn
(
    Entity entity,
    EntityStore& store,
    const std::string& componentName,
    const JsonValue& definitions,
    const JsonValue& compTemplate
)
{
    if (componentName == "renderable") {
        const char* modelName = compTemplate["model"].GetString();
        auto modelHandle = _gfxContext->findModel(modelName);
        if (modelHandle) {
            _renderGraph->cloneAndAddNode(entity,
                modelHandle->root(), nullptr);
        }
        else {
            CK_LOG_WARN("OverviewSample",
                        "Entity: %" PRIu64 ", Component %s: %s not found\n",
                        entity, componentName.c_str(), modelName);
        }
    }
    else if (componentName == "scenebody") {
        ove::SceneDataContext::SceneBodyInitParams initInfo;
        
        //  obtain collision shape info
        auto gfxNode = _renderGraph->findNode(entity);
        gfx::AABB nodeAABB;
        gfx::Vector3 nodeTranslate;
        if (gfxNode) {
            auto& transform = gfxNode->transform();
            cinek::gfx::generateAABBForNode(nodeAABB, *gfxNode.resource());
            nodeTranslate.x = transform[12];
            nodeTranslate.y = transform[13];
            nodeTranslate.z = transform[14];
        }
        else {
            //  create a placeholder shape for now.
            nodeAABB = gfx::AABB(1.0f);
            nodeTranslate.x = 0.0f;
            nodeTranslate.y = 0.0f;
            nodeTranslate.z = 0.0f;
        }
        
        initInfo.collisionShape = nullptr;
        
        auto dims = nodeAABB.dimensions();
        auto center = nodeAABB.center();
        
        btTransform localShapeTransform;
        localShapeTransform.getBasis().setIdentity();
        localShapeTransform.getOrigin().setValue(center.x - nodeTranslate.x,
                center.y - nodeTranslate.y,
                center.z - nodeTranslate.z);
        
        auto it = compTemplate.FindMember("shape");
        if (it != compTemplate.MemberEnd()) {
            const char* shapeType = it->value.GetString();
            if (!strcasecmp(shapeType, "cylinder")) {
                initInfo.collisionShape = _sceneDataContext->allocateCylinderShape(
                    btVector3(dims.x*0.5f, dims.y*0.5f, dims.z*0.5f),
                    localShapeTransform);
            }
            else if (!strcasecmp(shapeType, "box")) {
                initInfo.collisionShape = _sceneDataContext->allocateBoxShape(
                    btVector3(dims.x*0.5f, dims.y*0.5f, dims.z*0.5f),
                    localShapeTransform);
            }
            else {
                CK_LOG_WARN("OverviewSample",
                        "Entity: %" PRIu64 ", Component %s: invalid shape type %s. "
                        "Defauling to box.\n",
                        entity, componentName.c_str(), shapeType);
            }
        }
        else {
            CK_LOG_WARN("OverviewSample",
                        "Entity: %" PRIu64 ", Component %s: no valid shape entry. "
                        "Defauling to box.\n",
                        entity, componentName.c_str());
        }
        if (!initInfo.collisionShape) {
            initInfo.collisionShape = _sceneDataContext->allocateBoxShape(
                    btVector3(dims.x*0.5f, dims.y*0.5f, dims.z*0.5f),
                    localShapeTransform);
        }

        //  other properties
        it = compTemplate.FindMember("mass");
        if (it != compTemplate.MemberEnd()) {
            initInfo.mass = (float)it->value.GetDouble();
        }
        else {
            initInfo.mass = 0.0f;
        }
        
        btRigidBody* body = _sceneDataContext->allocateBody(initInfo, gfxNode);
        if (body) {
            _scene->attachBody(body, entity);
        }
        else {
            CK_LOG_WARN("OverviewSample",
                        "Entity: %" PRIu64 ", Component %s: failed to create body\n",
                        entity, componentName.c_str());
        }
    }
}

void GameEntityFactory::onCustomComponentDestroyFn
(
    EntityDataTable& table,
    ComponentRowIndex compRowIndex
)
{
}


}