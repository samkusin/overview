//
//  GameController.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/9/15.
//
//

#include "Engine/Physics/SceneDataContext.hpp"
#include "Engine/Physics/Scene.hpp"
#include "Engine/Physics/SceneDebugDrawer.hpp"
#include "Engine/Render/RenderGraph.hpp"
#include "Engine/EntityDatabase.hpp"

#include "GameEntityFactory.hpp"

#include "Views/StartupView.hpp"
#include "Views/LoadSceneView.hpp"
#include "Views/Game/GameView.hpp"

#include "PrototypeApplication.hpp"

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>
#include <vector>

namespace cinek {

PrototypeApplication::PrototypeApplication
(
    gfx::Context& gfxContext,
    const gfx::NodeRenderer::ProgramMap& programs,
    const gfx::NodeRenderer::UniformMap& uniforms,
    NVGcontext* nvg
) :
    _gfxContext(&gfxContext),
    _taskScheduler(64),
    _server(_messenger, { 64*1024, 64*1024 }),
    _client(_messenger, { 32*1024, 32*1024 }),
    _clientSender { &_client, _server.address() } ,
    _resourceFactory(_gfxContext, &_taskScheduler),
    _renderPrograms(programs),
    _renderUniforms(uniforms),
    _renderer(),
    _nvg(nvg)
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
    
    _renderContext.programs = &_renderPrograms;
    _renderContext.uniforms = &_renderUniforms;
    _renderContext.frameRect = gfx::Rect { 0,0,0,0 };
    
    ove::SceneDataContext::InitParams sceneDataInit;
    sceneDataInit.numBodies = 256;
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
        //  default
        cinek::EntityStore::InitParams {
                16384
        },
        //  staging
        cinek::EntityStore::InitParams {
                1024
        }
    };

    _entityDb = allocate_unique<ove::EntityDatabase>(entityStoreInitializers,
        *_componentFactory);

    
    //  define the top-level states for this application
    _appContext = allocate_unique<ApplicationContext>();
    
    _appContext->nvg = _nvg;
    _appContext->entityDatabase = _entityDb.get();
    _appContext->taskScheduler = &_taskScheduler;
    _appContext->resourceFactory = &_resourceFactory;
    _appContext->msgClientSender = &_clientSender;
    _appContext->scene = _scene.get();
    _appContext->sceneData = _sceneData.get();
    _appContext->sceneDebugDrawer = _sceneDbgDraw.get();
    _appContext->gfxContext = _gfxContext;
    _appContext->renderGraph = _renderGraph.get();
    _appContext->renderContext = &_renderContext;
    
    _viewStack.setFactory(
        [this](const std::string& viewName, ove::ViewController* )
            -> std::shared_ptr<ove::ViewController> {
            //  Startup View initialzes common data for the application
            //  Game View encompasses the whole game simulation
            //  Ship View encompasses the in-game ship mode
            //  Ship Bridge View encompasses the in-game ship bridge mode
            if (viewName == "StartupView") {
                return std::allocate_shared<StartupView>(
                    std_allocator<StartupView>(), _appContext.get());
            }
            else if (viewName == "GameView") {
                return std::allocate_shared<GameView>(
                    std_allocator<GameView>(), _appContext.get());
            }
            else if (viewName == "LoadSceneView") {
                return std::allocate_shared<LoadSceneView>(
                std_allocator<LoadSceneView>(), _appContext.get());

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
    
    _viewStack.simulate(dt);
}

void PrototypeApplication::renderFrame
(
    double dt,
    const gfx::Rect& viewRect,
    const cinek::uicore::InputState& inputState
)
{
    _taskScheduler.update(dt * 1000);
    
    _renderContext.frameRect = viewRect;
 
    _renderGraph->update(dt);
    
    _viewStack.frameUpdate(dt, inputState);
        
    _gfxContext->update();
}

void PrototypeApplication::endFrame()
{
    _viewStack.endFrame();

    _client.transmit();
    _server.transmit();

    _entityDb->gc();
}


}

