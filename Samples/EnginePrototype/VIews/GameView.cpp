//
//  GameView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

#include "GameView.hpp"

#include "UICore/UIBuilder.hpp"
#include "CKGfx/Light.hpp"

#include <bx/fpumath.h>
#include <bgfx/bgfx.h>

namespace cinek {

GameView::GameView(const ApplicationContext& api) :
    AppViewController(api),
    _selectedEntityTemplateIndex(-1)
{
    //  create our offscreen node graph for rendering models
    //
    gfx::NodeElementCounts counts;
    counts.meshNodeCount = 32;
    counts.lightNodeCount = 2;
    counts.transformNodeCount = 16;
    counts.armatureNodeCount = 1;
    counts.objectNodeCount = 1;
    
    _modelGraph = std::move(gfx::NodeGraph(counts));
    _modelGraph.setRoot(_modelGraph.createObjectNode(0));
    
    auto lightNode = _modelGraph.createLightNode();
    gfx::Light light;
    light.type = gfx::LightType::kAmbient;
    light.ambientComp = 1.0f;
    light.color = gfx::Color4(1.0f,1.0f,1.0f,1.0f).toABGR();
    lightNode->light()->light = renderService().createLight(light);
    _modelGraph.addChildNodeToNode(lightNode, _modelGraph.root());
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
    const ove::InputState& inputState
)
{
    //  generate UI
    uicore::Layout uiLayout;
    
    uiLayout.frame(kUIEvtId_GameView, UI_BUTTON0_DOWN, this, viewUIRenderHook, this, UI_FILL)
        .setSize(renderService().getViewRect().w, renderService().getViewRect().h)
        .column(UI_RIGHT | UI_VFILL)
            .setSize(400, 100)
            .listbox(this, kUIProviderId_EntityTemplates,
                uicore::ListboxLayout::kGrid,
                &_selectedEntityTemplateIndex)
        .end()
    .end();
    
    _camera.near = 0.1f;
    _camera.far = 1000.0f;
    _camera.fovDegrees = 60.0f;
    _camera.viewportRect = renderService().getViewRect();
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
    
    renderService().renderNode(_renderer, sceneService().getGfxRootNode(), _camera);
    
    sceneService().renderDebug(renderService(), _camera);
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

bool GameView::onUIDataItemRequest
(
    int id,
    int row,
    int col,
    uicore::DataObject& data
)
{
    return false;
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
