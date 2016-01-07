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
    AppViewController(api),
    _selectedEntityTemplateIndex(-1)
{
    
}

    
void GameView::onViewAdded(ove::ViewStack& stateController)
{
    assetService().loadManifest("scenes/apartment.json",
        [this](std::shared_ptr<ove::AssetManifest> manifest) {
            sceneService().initialize(manifest);
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
    
    uiLayout.frame(kUIEvtId_GameView, UI_BUTTON0_DOWN, this, viewUIRenderHook, this, UI_FILL)
        .setSize(renderService.getViewRect().w, renderService.getViewRect().h)
        .column(UI_RIGHT | UI_VFILL)
            .setSize(400, 100)
            .listbox(this, kUIProviderId_EntityTemplates,
                uicore::ListboxLayout::kGrid,
                &_selectedEntityTemplateIndex)
        .end()
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
    _camera.near = 0.1f;
    _camera.far = 1000.0f;
    _camera.fovDegrees = 60.0f;
    _camera.viewportRect = renderService.getViewRect();
    _freeCameraController.handleCameraInput(_camera, inputState, dt);
    _camera.update();

    int32_t vx = inputState.mx - _camera.viewportRect.x;
    int32_t vy = inputState.my - _camera.viewportRect.y;

    gfx::Vector3 dir = _camera.rayFromViewportCoordinate(vx, vy);
    gfx::Vector3 pos = _camera.worldPosition();
    /*
    printf("RayWorldOrigin = {%.2f,%.2f,%.2f}\n", pos.x, pos.y, pos.z);
    printf("RayWorldDir = {%.2f,%.2f,%.2f}\n", dir.x, dir.y, dir.z);
    */
    
    ove::SceneRayTestResult rayTestResult = sceneService().rayTestClosest({ pos.x, pos.y, pos.z },
        { dir.x , dir.y, dir.z },
        100.0f);
    
    
    sceneService().renderDebugAddRayTestHit(rayTestResult, { pos.x, pos.y, pos.z }, 0.1f, false);
    
    renderService.renderNodeWithCamera(sceneService().getGfxRootNode(), _camera);
    
    sceneService().renderDebug(renderService, _camera);
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
    int id,
    int index,
    uicore::DataObject& data
)
{

}


void GameView::onUIFrameEvent(int id, const uicore::FrameEvent& evt)
{
    //  frame's region is the same as the camera viewport
    if (id == kUIEvtId_GameView) {
        if (evt.evtType == UI_BUTTON0_DOWN) {
           
        }
    }
}

} /* namespace cinek */
