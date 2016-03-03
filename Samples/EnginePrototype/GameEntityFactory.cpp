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

#include "Game/NavDataContext.hpp"
#include "Engine/Controller/NavBody.hpp"
#include "Engine/Controller/NavSystem.hpp"
#include "Engine/Game/NavSceneBodyTransform.hpp"

#include <ckjson/json.hpp>

namespace cinek {

GameEntityFactory::GameEntityFactory
(
    gfx::Context* gfxContext,
    ove::SceneDataContext* sceneData,
    ove::Scene* scene,
    ove::RenderGraph* renderGraph,
    NavDataContext* navDataContext,
    ove::NavSystem* navSystem,
    TransformDataContext* transformDataContext
) :
    _gfxContext(gfxContext),
    _sceneDataContext(sceneData),
    _scene(scene),
    _renderGraph(renderGraph),
    _navDataContext(navDataContext),
    _navSystem(navSystem),
    _transformDataContext(transformDataContext){
}
                      

void GameEntityFactory::onCustomComponentCreateFn
(
    Entity entity,
    EntityStore& store,
    const std::string& templateName,
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

        ove::SceneBody* body = _sceneDataContext->allocateBody(initInfo, gfxNode, entity);
        if (body) {
            _scene->attachBody(body, ove::SceneBody::kIsObject);
            
            //  if navbody exists, then we still need to create the navbody transform
            //  - this would've been done during navbody component create, if
            //    the scenebody was previously created.  the step below takes care
            //    of the opposite case.
            ove::NavBody* navBody = _navSystem->findBody(entity);
            if (navBody) {
                navBody->setTransform(_navDataContext->allocateTransform(body, entity));
            }
        }
        else {
            CK_LOG_WARN("OverviewSample",
                        "Entity: %" PRIu64 ", Component %s: failed to create body\n",
                        entity, componentName.c_str());
        }
    }
    else if (componentName == "drivebody") {
        ove::NavBody::InitProperties initProps;

        if (compTemplate.HasMember("speed") && compTemplate["speed"].IsArray()) {
            const JsonValue& speed = compTemplate["speed"];
            
            initProps.speedLimit = ckm::scalar(speed[0U].GetDouble());
        }
        
        initProps.entity = entity;
        
        ove::NavBody* navBody = _navDataContext->allocateBody(initProps);
        if (navBody) {
            ove::SceneBody* sceneBody = _scene->findBody(entity);
            if (sceneBody) {
                navBody->setTransform(_navDataContext->allocateTransform(sceneBody, entity));
            }
            _navSystem->attachBody(navBody);
        }
        else {
            CK_LOG_WARN("OverviewSample",
                        "Entity: %" PRIu64 ", Component %s: failed to create body\n",
                        entity, componentName.c_str());
        }
    }
    else if (componentName == "animation") {
        if (compTemplate.HasMember("set")) {
            const JsonValue& setDefinitions = compTemplate["set"];
        
            //  create a new transform set using the template name as id
            
            //  every set
        }
    }
}

void GameEntityFactory::onCustomComponentEntityDestroyFn(Entity entity)
{
    //  iterate through all components
    //  TODO - perhaps we need to identify what components are attached to
    //         the entity for optimization
    
    //  destroy nav
    ove::NavBody* navBody = _navSystem->detachBody(entity);
    if (navBody) {
        _navDataContext->freeBody(navBody);
        navBody = nullptr;
    }
    //  destroy scene
    ove::SceneBody* body = _scene->detachBody(entity);
    if (body) {
        _sceneDataContext->freeBody(body);
        body = nullptr;
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
    ove::SceneBody* sceneBody = _scene->findBody(origin);
    if (sceneBody) {
        ove::SceneBody* clonedBody = _sceneDataContext->cloneBody(sceneBody, gfxNode, target);
        sceneBody = _scene->attachBody(clonedBody, clonedBody->categoryMask);
    }
    //  navbody
    ove::NavBody* navBody = _navSystem->findBody(origin);
    if (navBody) {
        ove::NavBody* clonedBody = _navDataContext->cloneBody(navBody, sceneBody, target);
        navBody = _navSystem->attachBody(clonedBody);
    }
}


}