//
//  Main.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/23/15.
//
//

#include "Common.hpp"
#include "Renderer.hpp"

#include "CKGfx/VertexTypes.hpp"
#include "CKGfx/ShaderLibrary.hpp"
#include "CKGfx/Context.hpp"
#include "CKGfx/ModelJsonSerializer.hpp"
#include "CKGfx/NodeGraph.hpp"
#include "CKGfx/NodeRenderer.hpp"
#include "CKGfx/Node.hpp"
#include "CKGfx/AnimationController.hpp"
#include "CKGfx/Light.hpp"

#include <cinek/file.hpp>
#include <cinek/allocator.hpp>
#include <cinek/objectpool.hpp>

#include <SDL2/SDL_timer.h>
#include <bgfx/bgfx.h>

#include "UICore/UITypes.hpp"
#include "UICore/UIEngine.hpp"
#include "UICore/UIRenderer.hpp"

#include "UICore/oui.h"

#include <unordered_map>

#include "Engine/EngineTypes.hpp"


////////////////////////////////////////////////////////////////////////////////
//  OverviewSample

#include "Views/GameView.hpp"
#include "Views/StartupView.hpp"

#include "ResourceFactory.hpp"
#include "SceneDebugDrawer.hpp"

#include "FreeCameraController.hpp"

#include "Engine/Messages/Core.hpp"
#include "Engine/Messages/Entity.hpp"
#include "Engine/Messages/Scene.hpp"

#include "Engine/Scenes/Scene.hpp"
#include "Engine/Scenes/SceneDataContext.hpp"
#include "Engine/Render/RenderGraph.hpp"

#include "Engine/Debug.hpp"
#include "Engine/SceneJsonLoader.hpp"
#include "Engine/ViewAPI.hpp"
#include "Engine/ViewStack.hpp"
#include "Engine/EntityDatabase.hpp"

#include "Engine/AssetManifestLoader.hpp"
#include "Engine/Comp/Transform.hpp"
#include "Engine/Tasks/LoadAssetManifest.hpp"

#include <cinek/taskscheduler.hpp>
#include <ckmsg/messenger.hpp>
#include <ckmsg/server.hpp>
#include <ckmsg/client.hpp>


/*  The Overview Sample illustrates core engine features and acts as a testbed
    for experimental features integrated into an application.
  
    Features Include:
    
    * ViewController Management
    * EntityStores
*/
class OverviewSample : public cinek::ove::EntityComponentFactory
{
public:
    OverviewSample
    (
        const cinek::gfx::NodeRenderer::ProgramMap& programs,
        const cinek::gfx::NodeRenderer::UniformMap& uniforms,
        cinek::gfx::Context& gfxContext
    );
    
    void startFrame();
    void simulate(double time, double dt);
    void endFrame(double dt, const cinek::ove::InputState& state);
    
    void render(cinek::gfx::NodeRenderer& renderer,
                const cinek::gfx::Rect& viewRect);
    
public:
    virtual void onCustomComponentCreateFn(cinek::Entity entity,
                        cinek::EntityStore& store,
                        const std::string& componentName,
                        const cinek::JsonValue& definitions,
                        const cinek::JsonValue& data) override;
    virtual void onCustomComponentDestroyFn(cinek::EntityDataTable& table,
                        cinek::ComponentRowIndex compRowIndex) override;
    
    
private:
    void loadEntityDefinitionsCmd(ckmsg::ServerRequestId reqId,
                                  const cinek::ove::EntityLoadDefinitionsRequest& req);
    
    void loadSceneCmd(ckmsg::ServerRequestId reqId,
                      const cinek::ove::SceneLoadRequest& req);
    
private:
    const cinek::gfx::NodeRenderer::ProgramMap* _programs;
    const cinek::gfx::NodeRenderer::UniformMap* _uniforms;
    cinek::gfx::Context* _gfxContext;
    
    cinek::ove::InputState _inputState;
    
    cinek::gfx::Camera _mainCamera;
    cinek::ove::FreeCameraController _cameraController;
    
    ckmsg::Messenger _messenger;
    cinek::ove::MessageServer _server;
    cinek::ove::MessageClient _client;
    cinek::ove::MessageClientSender _clientSender;

    cinek::TaskScheduler _scheduler;
    cinek::ove::ViewAPI _viewAPI;
    cinek::ove::ViewStack _viewStack;
    cinek::ove::EntityDatabase _entityDatabase;
    
    cinek::unique_ptr<cinek::ove::ResourceFactory> _resourceFactory;
    
    cinek::ove::AssetManifestLoader _loader;
    
    static constexpr double kActionFPS = 60.0;
    static constexpr double kSecsPerActionFrame = 1/kActionFPS;
    
    double _timeUntilActionFrame;
    cinek::unique_ptr<cinek::ove::SceneDebugDrawer> _sceneDbgDraw;
    cinek::unique_ptr<cinek::ove::SceneDataContext> _sceneData;
    cinek::unique_ptr<cinek::ove::Scene> _scene;
    
    ////////////////////////////////////////////////////////////////////////////

    cinek::unique_ptr<cinek::ove::RenderGraph> _renderGraph;

};

OverviewSample::OverviewSample
(
    const cinek::gfx::NodeRenderer::ProgramMap& programs,
    const cinek::gfx::NodeRenderer::UniformMap& uniforms,
    cinek::gfx::Context& gfxContext
) :
    _programs(&programs),
    _uniforms(&uniforms),
    _gfxContext(&gfxContext),
    _server(_messenger, { 64*1024, 64*1024 }),
    _client(_messenger, { 32*1024, 32*1024 }),
    _scheduler(64),
    _timeUntilActionFrame(0.0)
{
    _entityDatabase = std::move(
        cinek::ove::EntityDatabase({
            cinek::EntityStore::InitParams {
                1024,           // num entities
                {
                     cinek::ove::TransformComponent::makeDescriptor(1024)
                },
                {
                },
                12345678        // random seed
            }
        },
        *this)
    );
    
    _resourceFactory = cinek::allocate_unique<cinek::ove::ResourceFactory>(
        *_gfxContext,
        _scheduler
    );
    
    _clientSender.client = &_client;
    _clientSender.server = _server.address();
    
    static_assert(std::is_pod<cinek::ove::InputState>(), "Input State must be POD for now");
    memset(&_inputState, 0, sizeof(_inputState));
    
    cinek::gfx::Matrix4 cameraRotMtx;
    bx::mtxRotateXYZ(cameraRotMtx, 0, 0, 0);
    _cameraController.setTransform({ 0,2,-12}, cameraRotMtx);
 
    ////////////////////////////////////////////////////////////////////////////
    
    _server.on(cinek::ove::kMsgEntityLoadDefinitions,
        [this](ckmsg::ServerRequestId reqId, const ckmsg::Payload* payload) {
            loadEntityDefinitionsCmd(reqId,
                *reinterpret_cast<const cinek::ove::EntityLoadDefinitionsRequest*>(payload->data())
            );
        });
    
    _server.on(cinek::ove::kMsgSceneLoad,
        [this](ckmsg::ServerRequestId reqId, const ckmsg::Payload* payload) {
            loadSceneCmd(reqId,
                *reinterpret_cast<const cinek::ove::SceneLoadRequest*>(payload->data())
            );
            // TODO - initiate scene load flow
            // Load scene manifest
            // Build scene from manifest
            // Build node graph from manifest
        });
    
    ////////////////////////////////////////////////////////////////////////////
    
    //  create master scene
    cinek::gfx::NodeElementCounts sceneElementCounts;
    sceneElementCounts.meshNodeCount = 128;
    sceneElementCounts.armatureNodeCount = 32;
    sceneElementCounts.lightNodeCount = 8;
    sceneElementCounts.objectNodeCount = 64;
    sceneElementCounts.transformNodeCount = 64;
    
    _renderGraph = cinek::allocate_unique<cinek::ove::RenderGraph>(
        sceneElementCounts,
        1024,
        256
    );
    
    cinek::ove::SceneDataContext::InitParams sceneDataInit;
    sceneDataInit.numRigidBodies = 256;
    sceneDataInit.numTriMeshShapes = 32;
    
    _sceneDbgDraw = cinek::allocate_unique<cinek::ove::SceneDebugDrawer>();
    _sceneData = cinek::allocate_unique<cinek::ove::SceneDataContext>(sceneDataInit);
    _scene = cinek::allocate_unique<cinek::ove::Scene>(_sceneDbgDraw.get());
    
    _viewAPI = std::move(cinek::ove::ViewAPI(_viewStack,
                            _clientSender,
                            _entityDatabase,
                            *_scene.get())
                        );
    
    _viewStack.setFactory(
        [this](const std::string& viewName)
            -> cinek::unique_ptr<cinek::ove::ViewController>
        {
            if (viewName == "StartupView") {
                return cinek::allocate_unique<cinek::ove::StartupView>(_viewAPI);
            }
            else if (viewName == "GameView") {
                return cinek::allocate_unique<cinek::ove::GameView>(_viewAPI);
            }
            
            return nullptr;
        });
    
    _viewStack.load("StartupView");
    _viewStack.present("StartupView");
}

void OverviewSample::onCustomComponentCreateFn
(
    cinek::Entity entity,
    cinek::EntityStore& store,
    const std::string& componentName,
    const cinek::JsonValue& definitions,
    const cinek::JsonValue& data
)
{
    if (componentName == "renderable") {
        const char* modelName = data["model"].GetString();
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
        cinek::ove::SceneDataContext::SceneBodyInitParams initInfo;
        
        //  obtain collision shape info
        auto gfxNode = _renderGraph->findNode(entity);
        cinek::gfx::AABB nodeAABB;
        cinek::gfx::Vector3 nodeTranslate;
        if (gfxNode) {
            auto& transform = gfxNode->transform();
            cinek::gfx::generateAABBForNode(nodeAABB, *gfxNode.resource());
            nodeTranslate.x = transform[12];
            nodeTranslate.y = transform[13];
            nodeTranslate.z = transform[14];
        }
        else {
            //  create a placeholder shape for now.
            nodeAABB = cinek::gfx::AABB(1.0f);
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
        
        auto it = data.FindMember("shape");
        if (it != data.MemberEnd()) {
            const char* shapeType = it->value.GetString();
            if (!strcasecmp(shapeType, "cylinder")) {
                initInfo.collisionShape = _sceneData->allocateCylinderShape(
                    btVector3(dims.x*0.5f, dims.y*0.5f, dims.z*0.5f),
                    localShapeTransform);
            }
            else if (!strcasecmp(shapeType, "box")) {
                initInfo.collisionShape = _sceneData->allocateBoxShape(
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
            initInfo.collisionShape = _sceneData->allocateBoxShape(
                    btVector3(dims.x*0.5f, dims.y*0.5f, dims.z*0.5f),
                    localShapeTransform);
        }

        //  other properties
        it = data.FindMember("mass");
        if (it != data.MemberEnd()) {
            initInfo.mass = (float)it->value.GetDouble();
        }
        else {
            initInfo.mass = 0.0f;
        }
        
        btRigidBody* body = _sceneData->allocateBody(initInfo, gfxNode);
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

void OverviewSample::onCustomComponentDestroyFn
(
    cinek::EntityDataTable& table,
    cinek::ComponentRowIndex compRowIndex
)
{
}

void OverviewSample::loadEntityDefinitionsCmd
(
    ckmsg::ServerRequestId reqId,
    const cinek::ove::EntityLoadDefinitionsRequest& req
)
{
    //  TODO - let's consider a sequencer utility for generating
    //  sequences without resorting to brute force like what we're doing below.
    //
    auto taskCb = [this, reqId](cinek::Task::State state, cinek::Task& t) {
        cinek::ove::EntityLoadDefinitionsResponse resp;
        auto& task = reinterpret_cast<cinek::ove::LoadAssetManifest&>(t);
        ckmsg::Payload payload;
        if (state == cinek::Task::State::kEnded) {
            auto manifest = task.acquireManifest();
            strncpy(resp.name, manifest->name().c_str(), sizeof(resp.name));
            _entityDatabase.setManifest(std::move(manifest));
        }
        else {
            resp.name[0] = 0;
        }
        payload = cinek::ove::makePayloadFromData(resp);
        _server.reply(reqId, &payload);
    };
    
    auto task = cinek::allocate_unique<cinek::ove::LoadAssetManifest>(
        std::string(req.name, sizeof(req.name)), *_resourceFactory.get(), taskCb);
    
    _scheduler.schedule(std::move(task));
}

void OverviewSample::loadSceneCmd
(
    ckmsg::ServerRequestId reqId,
    const cinek::ove::SceneLoadRequest& req
)
{
    auto taskCb = [this, reqId](cinek::Task::State state, cinek::Task& t) {
        cinek::ove::SceneLoadResponse resp;
        auto& task = reinterpret_cast<cinek::ove::LoadAssetManifest&>(t);
        ckmsg::Payload payload;
        if (state == cinek::Task::State::kEnded) {
            auto manifest = task.acquireManifest();
            
            //  generate scene from the manifest
            cinek::ove::SceneJsonLoader loader(*_sceneData, *_gfxContext,
                                               _entityDatabase);
            
            loader(*_scene, *_renderGraph, manifest->root());
            
            //  prepare to send response
            strncpy(resp.name, manifest->name().c_str(), sizeof(resp.name));
        }
        else {
            resp.name[0] = 0;
        }
        payload = cinek::ove::makePayloadFromData(resp);
        _server.reply(reqId, &payload);
    };
    
    auto task = cinek::allocate_unique<cinek::ove::LoadAssetManifest>(
        std::string(req.name, sizeof(req.name)), *_resourceFactory.get(), taskCb);
    
    _scheduler.schedule(std::move(task));
}

void OverviewSample::startFrame()
{
    _server.receive();
    _client.receive();
    _viewStack.process();
}


void OverviewSample::simulate(double systemTime, double dt)
{
    _timeUntilActionFrame -= dt;
    while (_timeUntilActionFrame < 0)
    {
        _timeUntilActionFrame += kSecsPerActionFrame;
    }

    _viewStack.simulate(systemTime, dt);

    _scene->simulate(dt);
}


void OverviewSample::endFrame
(
    double dt,
    const cinek::ove::InputState& state
)
{
    _inputState = state;
    
    _viewStack.layout();
    _viewStack.frameUpdate(dt);
   
    _scheduler.update(dt * 1000);
    
    _entityDatabase.gc();
    
    _client.transmit();
    _server.transmit();
    
    _cameraController.handleCameraInput(_mainCamera, state, dt);
    
    _renderGraph->update(dt);
}


void OverviewSample::render
(
    cinek::gfx::NodeRenderer& renderer,
    const cinek::gfx::Rect& viewRect
)
{
    bgfx::setViewRect(0, viewRect.x, viewRect.y, viewRect.w, viewRect.h);
 
    _mainCamera.viewFrustrum = cinek::gfx::Frustrum(0.1, 100.0, M_PI * 60/180.0f,
        (float)viewRect.w/viewRect.h);
    
    renderer.setCamera(_mainCamera);
    renderer(_renderGraph->root());
    
    _sceneDbgDraw->setup(*_programs, *_uniforms, renderer.camera());
    _scene->debugRender();
}

////////////////////////////////////////////////////////////////////////////////
//
//  Main
//
////////////////////////////////////////////////////////////////////////////////

enum
{
    kShaderProgramStdMesh   = 0x00000001,
    kShaderProgramBoneMesh  = 0x00000002,
    kShaderProgramFlat      = 0x00000003
};

int runSample(int viewWidth, int viewHeight)
{
    //  UI
    UIcontext* uiContext = uiCreateContext(4096, 1<<20);
    uiMakeCurrent(uiContext);
    uiSetHandler(cinek::uicore::OUIHandler);
    
    NVGcontext* nvg = cinek::uicore::createRenderingContext(1);
    if (!nvg)
        return 1;
    
    //  GFX
    cinek::gfx::Rect viewRect = { 0, 0, viewWidth, viewHeight };
    cinek::gfx::VertexTypes::initialize();
    cinek::gfx::ShaderLibrary shaderLibrary;
    cinek::gfx::NodeRenderer::ProgramMap shaderPrograms;
    cinek::gfx::NodeRenderer::UniformMap shaderUniforms;
    
    registerShaders(
        shaderLibrary, shaderPrograms, shaderUniforms, {
            {
                cinek::gfx::kNodeProgramMesh, kShaderProgramStdMesh,
                "bin/vs_std_uv.bin",
                "bin/fs_std_tex.bin"
            },
            {
                cinek::gfx::kNodeProgramBoneMesh, kShaderProgramBoneMesh,
                "bin/vs_bone_uv.bin",
                "bin/fs_std_tex.bin"
            },
            {
                cinek::gfx::kNodeProgramFlat, kShaderProgramFlat,
                "bin/vs_flat_pos.bin",
                "bin/fs_flat_col.bin"
            }
        });

    cinek::gfx::Context::ResourceInitParams gfxInitParams;
    
    gfxInitParams.numMeshes = 1024;
    gfxInitParams.numMaterials = 1024;
    gfxInitParams.numTextures = 256;
    gfxInitParams.numAnimations = 256;
    gfxInitParams.numLights = 64;
    gfxInitParams.numModels = 64;
    
    cinek::gfx::Context gfxContext(gfxInitParams);
    
    OverviewSample application(shaderPrograms, shaderUniforms, gfxContext);
    
    //  Application
    
    /*
    auto sceneRoot = scene.root();
    cinek::gfx::NodeHandle newObjectNode;
    cinek::gfx::NodeHandle newObjectRoot;

    struct GfxNodeVisitContext
    {
        cinek::gfx::NodeId nodeId;
        cinek::gfx::AnimationControllerPool* ctrlPool;
        std::unordered_map<cinek::gfx::NodeId, cinek::gfx::AnimationControllerHandle>* ctrlMap;
    };

    newObjectNode = scene.clone(factorybot.root());
    newObjectRoot = scene.createObjectNode(100);
    bx::mtxSRT(newObjectRoot->transform(),
        1.0f, 1.0f, 1.0f,
        0, 0, 0,
        0, 0, -5);
    scene.addChildNodeToNode(newObjectNode, newObjectRoot);
    scene.addChildNodeToNode(newObjectRoot, scene.root());
    
    GfxNodeVisitContext nodeVisitCtx {
        newObjectRoot->objectNodeId(),
        &animControllerPool,
        &animControllers
    };
    
    cinek::gfx::visit(newObjectNode, [&nodeVisitCtx](cinek::gfx::NodeHandle node) -> bool {
        if (node->elementType() == cinek::gfx::Node::kElementTypeArmature) {
            cinek::gfx::AnimationController controller(node->armature()->animSet);
            auto animController = nodeVisitCtx.ctrlPool->add(std::move(controller));
            node->armature()->animController = animController;
            nodeVisitCtx.ctrlMap->emplace(nodeVisitCtx.nodeId, animController);
            animController->transitionToState("aggro");
        }
        return true;
    });
    */

    //  Renderer initialization
    cinek::gfx::NodeRenderer nodeRenderer(shaderPrograms, shaderUniforms);
    
    const double kSimFPS = 60.0;
    const double kSecsPerSimFrame = 1/kSimFPS;
    
    double simTime = 0.0;
    double lagSecsSim = 0.0;
    
    uint32_t systemTimeMs = SDL_GetTicks();
    bool running = true;
    
    cinek::ove::InputState polledInputState;
    
    while (running) {
        uint32_t nextSystemTimeMs = SDL_GetTicks();
        int32_t frameTimeMs = nextSystemTimeMs - systemTimeMs;
        systemTimeMs = nextSystemTimeMs;

        application.startFrame();
    
        //  TODO: Lag should not be incremented while Sim is Paused
        double frameTime = frameTimeMs*0.001;
        lagSecsSim += frameTime;
    
        ////////////////////////////////////////////////////////////////////////
        //  SIMULATION START (using a fixed timestep)
        //      All subsystems driven by the application simulation framerate.
        //
        while (lagSecsSim >= kSecsPerSimFrame)
        {
            application.simulate(simTime, kSecsPerSimFrame);

            lagSecsSim -= kSecsPerSimFrame;
            simTime += kSecsPerSimFrame;
            
            //  diagnostics.incrementRateGauge(Diagnostics::kFrameRate_Update);
        }
        //
        //  SIMULATION END
        ////////////////////////////////////////////////////////////////////////

        if (pollSDLEvents(polledInputState) & kPollSDLEvent_Quit)
            running = false;
        
        application.endFrame(frameTime, polledInputState);
        
        gfxContext.update();
        
        application.render(nodeRenderer, viewRect);

        cinek::uicore::render(nvg, viewRect);
    
        uiProcess(systemTimeMs);
        
        bgfx::frame();
    }

    cinek::uicore::destroyRenderingContext(nvg);
    
    return 0;
}



