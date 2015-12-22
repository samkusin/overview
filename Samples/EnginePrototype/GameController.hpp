//
//  GameController.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/9/15.
//
//

#ifndef Prototype_GameController_hpp
#define Prototype_GameController_hpp

#include "GameTypes.hpp"
#include "Common.hpp"
#include "ResourceFactory.hpp"
#include "FreeCameraController.hpp"
#include "Engine/EntityDatabase.hpp"
#include "Engine/Messages/Core.hpp"

#include "CKGfx/GfxTypes.hpp"
#include "CKGfx/NodeRenderer.hpp"
#include "CKGfx/Camera.hpp"

#include <cinek/allocator.hpp>
#include <cinek/taskscheduler.hpp>
#include <ckmsg/messenger.hpp>
#include <ckmsg/server.hpp>
#include <ckmsg/client.hpp>

#include <functional>
#include <string>

namespace cinek {

class GameControllerContext;
class GameEntityFactory;
    
class GameController
{
public:
    GameController
    (
        gfx::Context& gfxContext,
        const gfx::NodeRenderer::ProgramMap& programs,
        const gfx::NodeRenderer::UniformMap& uniforms
    );
    ~GameController();
    
    void beginFrame();
    
    void simulateFrame(double dt);
    void updateFrame(double dt, const ove::InputState& inputState);
    
    void renderFrame(const gfx::Rect& viewRect);
    void endFrame();
    
    //  initializes a scene
    void createScene(const ove::AssetManifest& manifest);
    
    //  Tears down all scene objects and data
    void destroyScene();
    
private:
    void switchToState(const std::string& nextState);
    unique_ptr<GameState> createState(const std::string& name);
    
private:
    gfx::Context* _gfxContext;
    TaskScheduler _taskScheduler;
    
    ckmsg::Messenger _messenger;
    ove::MessageServer _server;
    ove::MessageClient _client;
    ove::MessageClientSender _clientSender;

    ove::ResourceFactory _resourceFactory;
    
    gfx::NodeRenderer::ProgramMap _renderPrograms;
    gfx::NodeRenderer::UniformMap _renderUniforms;
    gfx::NodeRenderer _renderer;
    
    unique_ptr<ove::RenderGraph> _renderGraph;
    
    unique_ptr<ove::SceneDataContext> _sceneData;
    unique_ptr<ove::SceneDebugDrawer> _sceneDbgDraw;
    unique_ptr<ove::Scene> _scene;
    
    unique_ptr<GameEntityFactory> _componentFactory;
    unique_ptr<ove::EntityDatabase> _entityDb;
    
    gfx::Camera _camera;
    ove::FreeCameraController _freeCameraController;
    
    std::string _nextStateName;
    
    unique_ptr<GameControllerContext> _controllerContext;
    unique_ptr<GameState> _currentState;
    unique_ptr<GameState> _endingState;
};
    
}

#endif /* GameController_hpp */
