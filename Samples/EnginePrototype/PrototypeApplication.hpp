//
//  GameController.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/9/15.
//
//

#ifndef Prototype_Application_hpp
#define Prototype_Application_hpp

#include "GameTypes.hpp"
#include "Common.hpp"
#include "ResourceFactory.hpp"

#include "UICore/UIEngine.hpp"

#include "Engine/EntityDatabase.hpp"
#include "Engine/Messages/Core.hpp"
#include "Engine/Render/RenderContext.hpp"
#include "Engine/Controller/ControllerTypes.hpp"
#include "Engine/ViewStack.hpp"

#include "CKGfx/GfxTypes.hpp"
#include "CKGfx/NodeRenderer.hpp"

#include <cinek/allocator.hpp>
#include <cinek/taskscheduler.hpp>
#include <ckmsg/messenger.hpp>
#include <ckmsg/server.hpp>
#include <ckmsg/client.hpp>

#include <functional>
#include <string>

namespace cinek {

class GameEntityFactory;
struct ApplicationContext;
    
class PrototypeApplication
{
public:
    PrototypeApplication
    (
        gfx::Context& gfxContext,
        const gfx::NodeRenderer::ProgramMap& programs,
        const gfx::NodeRenderer::UniformMap& uniforms,
        NVGcontext* nvg,
        UIcontext* ouiContext
    );
    ~PrototypeApplication();
    
    void beginFrame();
    
    void simulateFrame(CKTimeDelta dt);
    void renderFrame(CKTimeDelta dt, const gfx::Rect& viewRect,
        const cinek::uicore::InputState& inputState);
    void endFrame();
    
private:
    gfx::Context* _gfxContext;
    
    //  important - Application context must be destroyed after all objects
    //  below are destroyed.
    unique_ptr<ApplicationContext> _appContext;
    
    TaskScheduler _taskScheduler;
    
    ckmsg::Messenger _messenger;
    ove::MessageServer _server;
    ove::MessageClient _client;
    ove::MessageClientSender _clientSender;

    ove::ResourceFactory _resourceFactory;
    
    gfx::NodeRenderer::ProgramMap _renderPrograms;
    gfx::NodeRenderer::UniformMap _renderUniforms;
    
    gfx::NodeRenderer _renderer;
    NVGcontext* _nvg;
    uicore::Context _uiContext;
    
    unique_ptr<ove::RenderGraph> _renderGraph;
    ove::RenderContext _renderContext;
    
    unique_ptr<ove::EntityDatabase> _entityDb;
    
    unique_ptr<ove::SceneDataContext> _sceneData;
    unique_ptr<ove::SceneDebugDrawer> _sceneDbgDraw;
    unique_ptr<ove::Scene> _scene;
    
    unique_ptr<ove::Pathfinder> _pathfinder;
    unique_ptr<ove::PathfinderDebug> _pathfinderDebug;
    
    unique_ptr<NavDataContext> _navDataContext;
    unique_ptr<ove::NavSystem> _navSystem;
    
    unique_ptr<TransformDataContext> _transformDataContext;
    unique_ptr<ove::TransformSystem> _transformSystem;
    
    unique_ptr<GameEntityFactory> _componentFactory;
    
    
    ove::ViewStack _viewStack;
};
    
}

#endif /* Prototype_Application_hpp */
