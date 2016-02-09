//
//  GamneEntityFactory.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/10/15.
//
//

#include "GameEntityFactory.hpp"
#include "Engine/Physics/Scene.hpp"
#include "Engine/Physics/SceneDataContext.hpp"
#include "Engine/Render/RenderGraph.hpp"
#include "CKGfx/ModelSet.hpp"
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
        const char* modelSetName = compTemplate["modelset"].GetString();
        const char* modelName = compTemplate["model"].GetString();
        
        gfx::ModelSetHandle modelSetHandle = _gfxContext->findModelSet(modelSetName);
        gfx::NodeHandle modelHandle;
        
        if (modelSetHandle) {
            modelHandle = modelSetHandle->find(modelName);
        }
        
        if (modelHandle) {
            _renderGraph->cloneAndAddNode(entity,
                modelHandle, nullptr);
        }
        else {
                CK_LOG_WARN("OverviewSample",
                        "Entity: %" PRIu64 ", Component %s: %s/%s not found\n",
                        entity, componentName.c_str(), modelSetName, modelName);
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
            nodeAABB = gfxNode->calculateAABB();
            //cinek::gfx::generateAABBForNode(nodeAABB, *gfxNode.resource());
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
        /*
        it = compTemplate.FindMember("mass");
        if (it != compTemplate.MemberEnd()) {
            initInfo.mass = (float)it->value.GetDouble();
        }
        else {
            initInfo.mass = 0.0f;
        }
        */
        
        ove::SceneBody* body = _sceneDataContext->allocateBody(initInfo, gfxNode, entity);
        if (body) {
            _scene->attachBody(body, ove::SceneBody::kIsObject);
            //body->setLinearVelocity(btVector3(1.0f, 3.0f, -3.0f));
        }
        else {
            CK_LOG_WARN("OverviewSample",
                        "Entity: %" PRIu64 ", Component %s: failed to create body\n",
                        entity, componentName.c_str());
        }
    }
}

void GameEntityFactory::onCustomComponentEntityDestroyFn(Entity entity)
{
    //  iterate through all components
    //  TODO - perhaps we need to identify what components are attached to
    //         the entity for optimization
    
    //  destroy scene
    ove::SceneBody* body = _scene->detachBody(entity);
    if (body) {
        _sceneDataContext->freeBody(body);
    }
    //  destroy renderable
    _renderGraph->removeNode(entity);
}

void GameEntityFactory::onCustomComponentEntityCloneFn
(
    Entity target,
    Entity origin
)
{
    //  clone all components
    //  TODO - perhaps we need to identify what components are attached to
    //         the entity for optimization
    
    //  renderable
    gfx::NodeHandle gfxNode = _renderGraph->findNode(origin);
    if (gfxNode) {
        CK_ASSERT_RETURN(gfxNode->elementType() == gfx::Node::kElementTypeObject);
        //  do not replicate the root object node, which identifies its
        //  entity.  but we still need to replicate its transform later
        //  Also make sure that our root object node is indeed an entity
        //  node, which is attached to the a single child
        CK_ASSERT_RETURN(gfxNode->firstChild() && !gfxNode->firstChild()->nextSibling());
        const gfx::Matrix4& mtx = gfxNode->transform();
        gfxNode = gfxNode->firstChildHandle();
        gfxNode = _renderGraph->cloneAndAddNode(target, gfxNode, nullptr);
        gfxNode->setTransform(mtx);
    }
    //  scene
    ove::SceneBody* body = _scene->findBody(origin);
    if (body) {
        ove::SceneBody* clonedBody = _sceneDataContext->cloneBody(body, gfxNode, target);
        body = _scene->attachBody(clonedBody, clonedBody->categoryMask);
    }
}


}