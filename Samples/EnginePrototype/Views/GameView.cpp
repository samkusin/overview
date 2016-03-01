//
//  GameView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

#include "EditorView.hpp"
#include "GameView.hpp"
#include "PlayView.hpp"

#include "Engine/Physics/Scene.hpp"
#include "Engine/Physics/SceneDebugDrawer.hpp"
#include "Engine/Nav/PathfinderDebug.hpp"
#include "Engine/Nav/Pathfinder.hpp"
#include "Engine/Render/RenderGraph.hpp"
#include "Engine/Render/RenderContext.hpp"
#include "Engine/AssetManifest.hpp"

#include "UICore/UIBuilder.hpp"
#include "CKGfx/Context.hpp"
#include "CKGfx/Light.hpp"
#include "CKGfx/ModelSet.hpp"
#include "CKGfx/Geometry.hpp"
#include "CKGfx/External/nanovg/nanovg.h"

#include <ckjson/json.hpp>
#include <ckm/math.hpp>

#include <bx/fpumath.h>
#include <bgfx/bgfx.h>

namespace cinek {

GameView::GameView(ApplicationContext* api) :
    AppViewController(api),
    _gameMode(GameMode::kNone)
{
    _gameViewContext.camera = &_camera;
    _gameViewContext.screenRayTestResult = &_viewToSceneRayTestResult;
    _gameViewContext.pathfinder = api->pathfinder;
    _gameViewContext.pathfinderDebug = api->pathfinderDebug;
    _gameViewContext.navSystem = api->navSystem;
    _gameViewContext.scene = api->scene;
    _gameViewContext.entityService = &entityService();
    _gameViewContext.assetService = &assetService();
    _gameViewContext.renderContext = api->renderContext;
    _gameViewContext.uiService = &uiService();
    _gameViewContext.nvgContext = nvgContext();
    _gameViewContext.game = this;
}

    
void GameView::onViewAdded(ove::ViewStack& stateController)
{
    //  As an AppViewController, GameView must initialize its states
    //  explicity upon switching from another application view.
    //
    //  Substates do not need to follow this rule.
    
    
    _editorView = std::allocate_shared<EditorView>(std_allocator<EditorView>(), &_gameViewContext);
    _playView = std::allocate_shared<PlayView>(std_allocator<PlayView>(), &_gameViewContext);
    
    //  initialize common objects for all substates
    _camera.near = 0.1f;
    _camera.far = 1000.0f;
    _camera.fovDegrees = 60.0f;
    _camera.worldMtx = gfx::Matrix4::kIdentity;
    
    _renderer.setPlaceholderDiffuseTexture(gfxContext().findTexture("textures/df_plh"));
    
    //  create player
    _focusedEntity = entityService().createEntity(kEntityStore_Staging, "entity",
                        "sample_male");
    
    auto body = scene().findBody(_focusedEntity);
    if (body) {
        body->setPosition(btVector3(0,0,0), btVector3(0,1,0));
    }
    
    //  game state machine
    _viewStack.setFactory(
        [this](const std::string& viewName, ove::ViewController* )
            -> std::shared_ptr<ove::ViewController> {
            //  Startup View initialzes common data for the application
            //  Game View encompasses the whole game simulation
            //  Ship View encompasses the in-game ship mode
            //  Ship Bridge View encompasses the in-game ship bridge mode
            if (viewName == "EditorView") {
                return _editorView;
            }
            else if (viewName == "PlayView") {
                return _playView;
            }
            return nullptr;
        });
    
    _viewStack.present("EditorView");
}

void GameView::onViewRemoved(ove::ViewStack& stateController)
{
    _viewStack.pop();
    
    entityService().destroyEntity(_focusedEntity);
    _focusedEntity = 0;
    
    _editorView = nullptr;
}

void GameView::onViewStartFrame(ove::ViewStack& stateController)
{
    _viewStack.startFrame();
}

void GameView::simulateView(ove::ViewStack& stateController, double dt)
{    
    _viewStack.simulate(dt);
}

void GameView::frameUpdateView
(
    ove::ViewStack& stateController,
    double dt,
    const cinek::uicore::InputState& inputState
)
{
    //  RENDERING
    _camera.viewportRect = renderContext().frameRect;
    _camera.update();
    
    int32_t vx = inputState.mx - _camera.viewportRect.x;
    int32_t vy = inputState.my - _camera.viewportRect.y;

    gfx::Vector3 dir = _camera.rayFromViewportCoordinate(vx, vy);
    gfx::Vector3 pos = _camera.worldPosition();
    
    _viewToSceneRayTestResult = scene().rayTestClosest(ove::btFromGfx(pos), ove::btFromGfx(dir), 100.0f);
    
    const ove::RenderContext& rc = renderContext();
    
    sceneDebug().setup(*rc.programs, *rc.uniforms, _camera);
    
    _renderer(*renderContext().programs, *renderContext().uniforms,
        _camera,
        renderGraph().root());
    
    sceneDebug().drawRayTestHit(_viewToSceneRayTestResult,
        { pos.x, pos.y, pos.z },
        btScalar(0.1),
        false);
    
    scene().debugRender();
    
    pathfinderDebug().setup(renderContext().programs,
        renderContext().uniforms,
        &_camera,
        nullptr);
    pathfinder().simulateDebug(pathfinderDebug());
    
    //  SUBSTATE UPDATES
    _viewStack.frameUpdate(dt, inputState);
}

void GameView::onViewEndFrame(ove::ViewStack& stateController)
{
    _viewStack.endFrame();
}

const char* GameView::viewId() const
{
    return "GameView";
}

void GameView::setGameMode(GameMode mode)
{
    _gameMode = mode;
}

GameMode GameView::getGameMode() const
{
    return _gameMode;
}

Entity GameView::getFocusedGameEntity() const
{
    return _focusedEntity;
}

} /* namespace cinek */
