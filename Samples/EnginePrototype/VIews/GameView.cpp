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
#include "CKGfx/ModelSet.hpp"

#include <bx/fpumath.h>
#include <bgfx/bgfx.h>

namespace cinek {

GameView::GameView(const ApplicationContext& api) :
    AppViewController(api),
    _selectedEntityTemplateIndex(-1)
{
   createModelStage();
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
            
            test1();
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
    
    ove::SceneRayTestResult rayTestResult = sceneService().rayTestClosest(
        { pos.x, pos.y, pos.z },
        { dir.x , dir.y, dir.z },
        100.0f);
    
    sceneService().renderDebugAddRayTestHit(rayTestResult,
        { pos.x, pos.y, pos.z }, 0.1f, false);
    
    renderService().renderNode(_renderer, sceneService().getGfxRootNode(), _camera);
    
    sceneService().renderDebug(renderService(), _camera);
    
    if (_testTarget)
    {
        bgfx::setViewRect(0, _camera.viewportRect.x, _camera.viewportRect.y,
            _camera.viewportRect.w, _camera.viewportRect.h);
        
        gfx::Color4 color { 1.0f, 1.0f, 1.0f, 1.0f };
        gfx::Vector4 param { 0.0f, 0.0f, 0.0f, 0.0f };
        
        bgfx::setUniform(renderService().bgfxUniformHandle(gfx::kNodeUniformMatSpecular), param);
        bgfx::setUniform(renderService().bgfxUniformHandle(gfx::kNodeUniformColor), color);
        
        bgfx::setTexture(0, renderService().bgfxUniformHandle(gfx::kNodeUniformTexDiffuse),
            _testTarget.texture()->bgfxHandle());

        gfx::Matrix4 worldTransform;
        bx::mtxSRT(worldTransform, 1, 1, 1, 0, 0, 0, 0, 6.0f, 2.0f);
        
        bgfx::setUniform(renderService().bgfxUniformHandle(gfx::kNodeUniformLightColor), color);
        
        bgfx::setUniform(renderService().bgfxUniformHandle(gfx::kNodeUniformLightDir), param);
        bgfx::setUniform(renderService().bgfxUniformHandle(gfx::kNodeUniformLightOrigin), param);
        bgfx::setUniform(renderService().bgfxUniformHandle(gfx::kNodeUniformLightCoeffs), param);
        param = { 1.0f, 0, 0, 0 };
        bgfx::setUniform(renderService().bgfxUniformHandle(gfx::kNodeUniformLightParam), param);
   
        bgfx::setVertexBuffer(_testQuadMesh.vertexBuffer());
        bgfx::setIndexBuffer(_testQuadMesh.indexBuffer());
        
        bgfx::setTransform(worldTransform);
        
          
        bgfx::setState(0
						| BGFX_STATE_RGB_WRITE
						| BGFX_STATE_ALPHA_WRITE
						| BGFX_STATE_DEPTH_WRITE
						| BGFX_STATE_DEPTH_TEST_LESS
						| BGFX_STATE_MSAA
                        | BGFX_STATE_CULL_CW
						);

        bgfx::submit(0, renderService().bgfxProgramHandle(gfx::kNodeProgramMesh));
    }
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

void GameView::createModelStage()
{
    //  create our offscreen node graph for rendering models
    //  uses a simple ambient light - might add another light
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

////////////////////////////////////////////////////////////////////////

void GameView::test1()
{
    auto modelset = renderService().findModelSet("models/simple.json");
    if (modelset) {
        auto model = modelset->find("DumbSlinky");
        _modelGraph.addChildNodeToNode(_modelGraph.clone(model), _modelGraph.root());
    }

    _testQuadMesh = gfx::createQuad(2.0f, gfx::VertexTypes::kVNormal_Tex0);

    _modelStageCamera.viewIndex = 2;
    _modelStageCamera.near = 0.1f;
    _modelStageCamera.far = 100.f;
    _modelStageCamera.fovDegrees = 60.0f;
    _modelStageCamera.viewportRect = gfx::Rect { 0, 0, 256, 256 };
    bx::mtxSRT(_modelStageCamera.worldMtx, 1, 1, 1, 0, 0, 0, 0, 0, -2.0f);
    _modelStageCamera.update();
    
    _testTarget = gfx::MultiTextureRenderTarget::createWithDepthTarget(
        _modelStageCamera.viewportRect.w,
        _modelStageCamera.viewportRect.h,
        bgfx::TextureFormat::BGRA8, bgfx::TextureFormat::D16);
    

    renderService().renderNodeToTarget(_renderer,
        (const gfx::RenderTarget&)_testTarget,
        _modelGraph.root(),
        _modelStageCamera);
}

} /* namespace cinek */
