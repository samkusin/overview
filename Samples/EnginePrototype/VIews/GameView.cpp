//
//  GameView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

#include "GameView.hpp"

#include "UICore/UIBuilder.hpp"

#include <bx/fpumath.h>

namespace cinek {

GameView::GameView(const ApplicationContext& api) :
    AppViewController(api)
{
    
}

    
void GameView::onViewAdded(ove::ViewStack& stateController)
{
    assetService().loadManifest("scenes/apartment.json",
        [this](std::shared_ptr<ove::AssetManifest> manifest) {
            sceneService().initializeScene(manifest);
            entityService().createEntity(0, "entity", "test_bot");
            
            cinek::gfx::Matrix4 cameraRotMtx;
            bx::mtxRotateXYZ(cameraRotMtx, 0, 0, 0);
            _freeCameraController.setTransform({ 0,2,-12}, cameraRotMtx);
        });
    
        

}

void GameView::onViewRemoved(ove::ViewStack& stateController)
{
}

void GameView::onViewForeground(ove::ViewStack& stateController)
{
}

void GameView::onViewBackground(ove::ViewStack& stateController)
{
}

void GameView::onViewStartFrame(ove::ViewStack& stateController)
{
}

void GameView::simulateView(ove::ViewStack& stateController, double dt)
{
}

void GameView::frameUpdateView
(
    ove::ViewStack& stateController,
    double dt,
    const ove::InputState& inputState,
    ove::RenderService& renderService
)
{
    uicore::Layout uiLayout;
    
    uiLayout.frame(viewUIRenderHook, this)
        .setEvents(UI_BUTTON0_DOWN, this, kUIEvtId_GameView)
        .setSize(renderService.getViewWidth(), renderService.getViewHeight())
        .end();
    
    /*
    layout.frame(layout_flags).events(UI_BUTTON0_DOWN).size(w,h).
        end();
        
    layout.frame().events(UI_BUTTON0_DOWN).size(w,h).
        column(UI_TOP|UI_HFILL).margins(10,10,10,10).
            button(BND_ICON_GHOST, "Item 1", ui_subscriber, ID_OK).
            hgroup().margins(5,5,5,5).
                radio(BND_ICON_REC, "Radio 1", &radio1).
                radio(BND_ICON_REC, "Radio 2", &radio1).
                radio(BND_ICON_REC, "Radio 3", &radio1).
            end().
        end().
        row().
            ...
        end().
    end();
 
*/
    _camera.viewFrustrum = cinek::gfx::Frustrum(0.1, 100.0, M_PI * 60/180.0f,
        (float)renderService.getViewWidth()/renderService.getViewHeight());
  
    _freeCameraController.handleCameraInput(_camera, inputState, dt);

    renderService.renderNodeWithCamera(sceneService().getGfxRootNode(), _camera);
    sceneService().renderSceneDebug(renderService, _camera);
}

void GameView::onViewEndFrame(ove::ViewStack& stateController)
{
}

const char* GameView::viewId() const
{
    return "GameView";
}

////////////////////////////////////////////////////////////////////////

void GameView::onUIEvent(int evtId, UIevent evtType, const uicore::UIeventdata& data)
{
}

void GameView::viewUIRenderHook(void* context, NVGcontext* nvg)
{
}


} /* namespace cinek */
