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
#include "Engine/EntityDatabase.hpp"
#include "Engine/Messages/Core.hpp"
#include "Engine/Render/RenderContext.hpp"
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
    
class PrototypeApplication
{
public:
    PrototypeApplication
    (
        gfx::Context& gfxContext,
        const gfx::NodeRenderer::ProgramMap& programs,
        const gfx::NodeRenderer::UniformMap& uniforms
    );
    ~PrototypeApplication();
    
    void beginFrame();
    
    void simulateFrame(double dt);
    void renderFrame(double dt, const gfx::Rect& viewRect,
        const ove::InputState& inputState);
    void endFrame();
    
private:
    gfx::Context* _gfxContext;
    TaskScheduler _taskScheduler;
    
    ove::ViewStack _viewStack;
    
    ckmsg::Messenger _messenger;
    ove::MessageServer _server;
    ove::MessageClient _client;
    ove::MessageClientSender _clientSender;

    ove::ResourceFactory _resourceFactory;
    
    gfx::NodeRenderer::ProgramMap _renderPrograms;
    gfx::NodeRenderer::UniformMap _renderUniforms;
    
    unique_ptr<ove::RenderGraph> _renderGraph;
    
    unique_ptr<ove::SceneDataContext> _sceneData;
    unique_ptr<ove::SceneDebugDrawer> _sceneDbgDraw;
    unique_ptr<ove::Scene> _scene;
    
    unique_ptr<GameEntityFactory> _componentFactory;
    unique_ptr<ove::EntityDatabase> _entityDb;
    
    gfx::NodeRenderer _renderer;
    ove::RenderContext _renderContext;
};
    
}

#endif /* Prototype_Application_hpp */
