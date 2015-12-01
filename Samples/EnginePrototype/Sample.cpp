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
#include "Engine/Render/RenderGraph.inl"


////////////////////////////////////////////////////////////////////////////////
//  OverviewSample

#include "Views/GameView.hpp"
#include "Views/StartupView.hpp"

#include "ResourceFactory.hpp"

#include "Engine/Messages/Core.hpp"
#include "Engine/Messages/Entity.hpp"

#include "Engine/Scenes/Scene.inl"
#include "Engine/Scenes/BulletPhysicsScene.hpp"
#include "Engine/ViewAPI.hpp"
#include "Engine/ViewStack.hpp"
#include "Engine/EntityDatabase.hpp"

#include "Engine/AssetManifestLoader.hpp"
#include "Engine/Comp/Transform.hpp"
#include "Engine/Tasks/LoadAssetManifest.hpp"
#include "Engine/Tasks/LoadAssets.hpp"

#include <cinek/taskscheduler.hpp>
#include <ckmsg/messenger.hpp>
#include <ckmsg/server.hpp>
#include <ckmsg/client.hpp>


template class cinek::ove::Scene<cinek::ove::BulletPhysicsScene>;

using GameScene = cinek::ove::Scene<cinek::ove::BulletPhysicsScene>;


/*  The Overview Sample illustrates core engine features and acts as a testbed
    for experimental features integrated into an application.
  
    Features Include:
    
    * ViewController Management
    * EntityStores
*/
class OverviewSample : public cinek::ove::EntityComponentFactory
{
public:
    OverviewSample(cinek::gfx::Context& gfxContext);
    
    void startFrame();
    void simulate(double time, double dt);
    void endFrame(double dt);
    
    void render(cinek::gfx::NodeRenderer& renderer);
    
    
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
    
private:
    cinek::gfx::Context* _gfxContext;
    
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
    cinek::unique_ptr<GameScene> _scene;
    
    ////////////////////////////////////////////////////////////////////////////

    cinek::unique_ptr<cinek::ove::RenderGraph> _renderGraph;

};

struct PrepareEntityForRender
{
    void operator()(cinek::gfx::Node& node, void* context);
    operator bool() const { return true; }
};


OverviewSample::OverviewSample(cinek::gfx::Context& gfxContext) :
    _server(_messenger, { 64*1024, 64*1024 }),
    _client(_messenger, { 32*1024, 32*1024 }),
    _scheduler(64),
    _timeUntilActionFrame(0.0),
    _gfxContext(&gfxContext)
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
 
    ////////////////////////////////////////////////////////////////////////////
    _server.on(cinek::ove::kMsgEntityLoadDefinitions,
        [this](ckmsg::ServerRequestId reqId, const ckmsg::Payload* payload) {
            loadEntityDefinitionsCmd(reqId,
                *reinterpret_cast<const cinek::ove::EntityLoadDefinitionsRequest*>(payload->data())
            );
        });
    
    /*
    _server.on(kAlphaRequest,
        [this](ckmsg::ServerRequestId reqId, const ckmsg::Payload* ) {

            _server.reply(reqId);
        });
    _server.on(kBetaRequest,
        [this](ckmsg::ServerRequestId reqId, const ckmsg::Payload* payload) {
            BetaPayload beta = *reinterpret_cast<const BetaPayload*>(payload->data());

            beta.y -= beta.z*2;
            ckmsg::Payload reply(reinterpret_cast<uint8_t*>(&beta), sizeof(beta));
            _server.reply(reqId, &reply);
        });
    _server.on(kGammaRequest,
        [this](ckmsg::ServerRequestId reqId, const ckmsg::Payload* payload) {
            GammaPayload gamma = *reinterpret_cast<const GammaPayload*>(payload->data());

            gamma.beta.z -= gamma.beta.z;
            strncpy(gamma.name, "good", sizeof(gamma.name));
            ckmsg::Payload reply(reinterpret_cast<uint8_t*>(&gamma), sizeof(gamma));
            _server.reply(reqId, &reply);
        });
    */
    
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
    
    _scene = cinek::allocate_unique<GameScene>();
    
    _viewAPI = std::move(cinek::ove::ViewAPI(_viewStack, _clientSender, _entityDatabase));
    
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
        if (state == cinek::Task::State::kEnded) {
            if (t.classId() == cinek::ove::LoadAssetManifest::kUUID) {
                auto& task = reinterpret_cast<cinek::ove::LoadAssetManifest&>(t);
                auto& nextTask = *reinterpret_cast<cinek::ove::LoadAssets *>(task.getNextTask());
                nextTask.setManifest(task.acquireManifest(), *_resourceFactory.get());
            }
            else if (t.classId() == cinek::ove::LoadAssets::kUUID) {
                cinek::ove::EntityLoadDefinitionsResponse resp;
                auto& task = reinterpret_cast<cinek::ove::LoadAssets&>(t);
        
                auto manifest = task.acquireManifest();
                manifest->name().copy(resp.name, sizeof(resp.name));
                _entityDatabase.setManifest(std::move(manifest));
                
                auto payload = cinek::ove::makePayloadFromData(resp);
                _server.reply(reqId, &payload);
            }
        }
        else {
            cinek::ove::EntityLoadDefinitionsResponse resp;
            resp.name[0] = 0;
            auto payload = cinek::ove::makePayloadFromData(resp);
            _server.reply(reqId, &payload);
        }
    };
    
    auto task = cinek::allocate_unique<cinek::ove::LoadAssetManifest>(
        std::string(req.name, sizeof(req.name)), taskCb);
    task->setNextTask(cinek::allocate_unique<cinek::ove::LoadAssets>(taskCb));
    
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

void PrepareEntityForRender::operator()
(
    cinek::gfx::Node &node,
    void* context
)
{
}

void OverviewSample::endFrame(double dt)
{
    _viewStack.layout();
    _viewStack.frameUpdate(dt);
 
    _renderGraph->prepare(dt, PrepareEntityForRender());
    
    _scheduler.update(dt * 1000);
    
    _entityDatabase.gc();
    
    _client.transmit();
    _server.transmit();
}


void OverviewSample::render(cinek::gfx::NodeRenderer& renderer)
{
    renderer(_renderGraph->root());
}


////////////////////////////////////////////////////////////////////////////////
//
//  Main
//
////////////////////////////////////////////////////////////////////////////////

enum
{
    kShaderProgramStdMesh   = 0x00000001,
    kShaderProgramBoneMesh  = 0x00000002
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
                cinek::gfx::kNodeProgramBoneMesh,kShaderProgramBoneMesh,
                "bin/vs_bone_uv.bin",
                "bin/fs_std_tex.bin"
            }
        });

    cinek::gfx::Context::ResourceInitParams gfxInitParams;
    
    gfxInitParams.numMeshes = 1024;
    gfxInitParams.numMaterials = 1024;
    gfxInitParams.numTextures = 256;
    gfxInitParams.numAnimations = 256;
    gfxInitParams.numLights = 64;
    
    cinek::gfx::Context gfxContext(gfxInitParams);
    
    OverviewSample application(gfxContext);
    
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
    cinek::gfx::Camera mainCamera;
    mainCamera.viewFrustrum = cinek::gfx::Frustrum(0.1, 100.0, M_PI * 60/180.0f,
        (float)viewWidth/viewHeight);
    
    bx::mtxSRT(mainCamera.worldMtx, 1, 1, 1, 0, 0, 0, 0, 2, -12);
    
    const double kSimFPS = 60.0;
    const double kSecsPerSimFrame = 1/kSimFPS;
    
    double simTime = 0.0;
    double lagSecsSim = 0.0;
    
    uint32_t systemTimeMs = SDL_GetTicks();
    bool running = true;
    
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

        PollStateSDL polledInputState;
        
        if (pollSDLEvents(polledInputState) & kPollSDLEvent_Quit)
            running = false;
        
        application.endFrame(frameTime);
        
        gfxContext.update();
        
        bgfx::setViewRect(0, viewRect.x, viewRect.y, viewRect.w, viewRect.h);
        
        nodeRenderer.setCamera(mainCamera);
        application.render(nodeRenderer);

        cinek::uicore::render(nvg, viewRect);
    
        uiProcess(systemTimeMs);
        
        bgfx::frame();
    }

    cinek::uicore::destroyRenderingContext(nvg);
    
    return 0;
}



