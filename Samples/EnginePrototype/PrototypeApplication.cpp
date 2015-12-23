//
//  GameController.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/9/15.
//
//

#include "Engine/Scenes/SceneDataContext.hpp"
#include "Engine/Scenes/Scene.hpp"
#include "Engine/Scenes/SceneDebugDrawer.hpp"
#include "Engine/Render/RenderGraph.hpp"
#include "Engine/EntityDatabase.hpp"

#include "GameEntityFactory.hpp"

#include "PrototypeApplication.hpp"

#include "Views/StartupView.hpp"
#include "Views/GameView.hpp"


#include <bgfx/bgfx.h>
#include <bx/fpumath.h>
#include <vector>

namespace cinek {

PrototypeApplication::PrototypeApplication
(
    gfx::Context& gfxContext,
    const gfx::NodeRenderer::ProgramMap& programs,
    const gfx::NodeRenderer::UniformMap& uniforms
) :
    _gfxContext(&gfxContext),
    _taskScheduler(64),
    _server(_messenger, { 64*1024, 64*1024 }),
    _client(_messenger, { 32*1024, 32*1024 }),
    _clientSender { &_client, _server.address() } ,
    _resourceFactory(*_gfxContext, _taskScheduler),
    _renderPrograms(programs),
    _renderUniforms(uniforms),
    _renderer()
{
    gfx::NodeElementCounts sceneElementCounts;
    sceneElementCounts.meshNodeCount = 128;
    sceneElementCounts.armatureNodeCount = 32;
    sceneElementCounts.lightNodeCount = 8;
    sceneElementCounts.objectNodeCount = 64;
    sceneElementCounts.transformNodeCount = 64;
    
    _renderGraph = allocate_unique<cinek::ove::RenderGraph>(
        sceneElementCounts,
        1024,
        256
    );
    
    ove::SceneDataContext::InitParams sceneDataInit;
    sceneDataInit.numRigidBodies = 256;
    sceneDataInit.numTriMeshShapes = 32;
    sceneDataInit.numCylinderShapes = 32;
    sceneDataInit.numBoxShapes = 32;
    
    _sceneData = cinek::allocate_unique<ove::SceneDataContext>(sceneDataInit);
    _sceneDbgDraw = cinek::allocate_unique<ove::SceneDebugDrawer>();
    _scene = cinek::allocate_unique<ove::Scene>(_sceneDbgDraw.get());

    
    _componentFactory = allocate_unique<GameEntityFactory>(
        *_gfxContext,
        *_sceneData,
        *_scene,
        *_renderGraph
    );
    
    std::vector<EntityStore::InitParams> entityStoreInitializers = {
        cinek::EntityStore::InitParams {
                1024,           // num entities
                {
                },
                {
                },
                12345678        // random seed
        }
    };

    _entityDb = allocate_unique<ove::EntityDatabase>(entityStoreInitializers,
        *_componentFactory);

    cinek::gfx::Matrix4 cameraRotMtx;
    bx::mtxRotateXYZ(cameraRotMtx, 0, 0, 0);
    _freeCameraController.setTransform({ 0,2,-12}, cameraRotMtx);
    
    //  define the top-level states for this application
    ApplicationContext context;
    
    context.entityDatabase = _entityDb.get();
    context.taskScheduler = &_taskScheduler;
    context.resourceFactory = &_resourceFactory;
    context.msgClientSender = &_clientSender;
    context.scene = _scene.get();
    context.sceneData = _sceneData.get();
    context.gfxContext = _gfxContext;
    context.renderGraph = _renderGraph.get();
    
    _viewStack.setFactory(
        [context](const std::string& viewName, ove::ViewController* ) -> unique_ptr<ove::ViewController> {
            //  Startup View initialzes common data for the application
            //  Game View encompasses the whole game simulation
            //  Ship View encompasses the in-game ship mode
            //  Ship Bridge View encompasses the in-game ship bridge mode
            if (viewName == "StartupView") {
                return allocate_unique<StartupView>(context);
            }
            else if (viewName == "GameView") {
                return allocate_unique<GameView>(context);
            }
            
            return nullptr;
        });
    
    _viewStack.present("StartupView");
}

PrototypeApplication::~PrototypeApplication()
{
}
    
void PrototypeApplication::beginFrame()
{
    _server.receive();
    _client.receive();

    _viewStack.startFrame();
}

void PrototypeApplication::simulateFrame(double dt)
{
    _scene->simulate(dt);

    _taskScheduler.update(dt * 1000);
    
    _viewStack.simulate(dt);
}

void PrototypeApplication::updateFrame
(
    double dt,
    const ove::InputState& inputState
)
{
    _viewStack.frameUpdate(dt);
    
    _freeCameraController.handleCameraInput(_camera, inputState, dt);

    _renderGraph->update(dt);
}

void PrototypeApplication::renderFrame(const gfx::Rect& viewRect)
{
    bgfx::setViewRect(0, viewRect.x, viewRect.y, viewRect.w, viewRect.h);

    _camera.viewFrustrum = cinek::gfx::Frustrum(0.1, 100.0, M_PI * 60/180.0f,
        (float)viewRect.w/viewRect.h);
    
    _renderer.setCamera(_camera);
    
    _renderer(_renderPrograms, _renderUniforms, _renderGraph->root());
    
    _sceneDbgDraw->setup(_renderPrograms, _renderUniforms, _renderer.camera());
    _scene->debugRender();
}

void PrototypeApplication::endFrame()
{
    _viewStack.endFrame();

    _client.transmit();
    _server.transmit();

    _entityDb->gc();
}


void PrototypeApplication::createScene
(
    const ove::AssetManifest& manifest
)
{
}

void PrototypeApplication::destroyScene()
{
}

}

