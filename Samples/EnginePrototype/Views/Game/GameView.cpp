//
//  GameView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

#include "EditorView.hpp"
#include "GameView.hpp"

#include "Engine/AssetManifest.hpp"
#include "UICore/UIBuilder.hpp"
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
    _gameViewContext {
        &_camera,
        &_viewToSceneRayTestResult,
        &sceneService(),
        &entityService(),
        &renderService(),
        &assetService(),
        nvgContext()
    }
{
}

    
void GameView::onViewAdded(ove::ViewStack& stateController)
{
    //  As an AppViewController, GameView must initialize its states
    //  explicity upon switching from another application view.
    //
    //  Substates do not need to follow this rule.
    
    
    _editorView = std::allocate_shared<EditorView>(std_allocator<EditorView>(), &_gameViewContext);
    
    //  initialize common objects for all substates
    _camera.near = 0.1f;
    _camera.far = 1000.0f;
    _camera.fovDegrees = 60.0f;
    _camera.worldMtx = gfx::Matrix4::kIdentity;
    
    _renderer.setPlaceholderDiffuseTexture(renderService().findTexture("textures/df_plh.png"));
    
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
            
            return nullptr;
        });
    
    _viewStack.present("EditorView");
}

void GameView::onViewRemoved(ove::ViewStack& stateController)
{
    _viewStack.pop();
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
    _camera.viewportRect = renderService().getViewRect();
    _camera.update();
    
    int32_t vx = inputState.mx - _camera.viewportRect.x;
    int32_t vy = inputState.my - _camera.viewportRect.y;

    gfx::Vector3 dir = _camera.rayFromViewportCoordinate(vx, vy);
    gfx::Vector3 pos = _camera.worldPosition();
    
    _viewToSceneRayTestResult = sceneService().rayTestClosest(
        { pos.x, pos.y, pos.z },
        { dir.x , dir.y, dir.z },
        100.0f);

    sceneService().renderDebugStart(renderService(), _camera);
    
    renderService().renderNode(_renderer, sceneService().getGfxRootNode(), _camera);
    
    sceneService().renderDebugAddRayTestHit(_viewToSceneRayTestResult,
        { pos.x, pos.y, pos.z }, 0.1f, false);
    
    sceneService().renderDebugEnd();
    
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

} /* namespace cinek */
