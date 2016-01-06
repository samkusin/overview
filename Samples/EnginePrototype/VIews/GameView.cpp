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
    
    uiLayout.frame(kUIEvtId_GameView, UI_BUTTON0_DOWN, this, viewUIRenderHook, this)
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
    //  todo - perfhaps a convenience method to automate this step?
    _camera.viewFrustrum = cinek::gfx::Frustrum(0.1, 100.0, M_PI * 60/180.0f,
        (float)renderService.getViewWidth()/renderService.getViewHeight());
  
    _freeCameraController.handleCameraInput(_camera, inputState, dt);
    _camera.update();

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

void GameView::viewUIRenderHook(void* context, NVGcontext* nvg)
{
}

void GameView::onUIDataItemRequest
(
    int item,
    int index,
    uicore::DataObject& data
)
{
}


void GameView::onUIDataUpdateItemAnchor
(
    int item,
    int index,
    const UIvec2& anchor,
    const UIvec2& dimensions
)
{
}

void GameView::onUIFrameEvent(int id, const uicore::FrameEvent& evt)
{
    if (id == kUIEvtId_GameView) {
        if (evt.evtType == UI_BUTTON0_DOWN) {
            gfx::Vector4 hitPt = {
                (2.0f * evt.cursor.x) / evt.size.x - 1.0f,
                1.0f - (2.0f*evt.cursor.y)/evt.size.y,
                1.0f,
                1.0f
            };
            
            gfx::Vector4 eyePt;
            bx::vec4MulMtx(eyePt, hitPt, _camera.invProjMtx);
            
            printf("HitPoint = {%.2f,%.2f,%.2f}\n", hitPt.x, hitPt.y, hitPt.z);
            printf("HitEye = {%.2f,%.2f,%.2f}\n", eyePt.x, eyePt.y, eyePt.z);
        }
    }
}

} /* namespace cinek */
