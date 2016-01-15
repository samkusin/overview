//
//  GameView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

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

GameView::GameView(const ApplicationContext& api) :
    AppViewController(api),
    _sceneLoaded(false),
    _mode(EditorMode::kNull),
    _stagedEntity(0),
    _shiftModifierAction(false),
    _displayTemplateSelector(false)
{
    _entityTemplateListboxState.highlightItem = -1;
}

    
void GameView::onViewAdded(ove::ViewStack& stateController)
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
    
    _modelStageGraph = allocate_unique<gfx::NodeGraph>(counts);
    auto root = _modelStageGraph->createObjectNode(0);
    bx::mtxIdentity(root->transform());
    _modelStageGraph->setRoot(root);
    
    gfx::Light light;
    light.type = gfx::LightType::kAmbient;
    light.ambientComp = 0.50f;
    light.color = gfx::Color4(1.0f,1.0f,1.0f,1.0f).toABGR();
    auto lightNode = _modelStageGraph->createLightNode();
    lightNode->light()->light = renderService().createLight(light);
    _modelStageGraph->addChildNodeToNode(lightNode, _modelStageGraph->root());
    
    light.type = gfx::LightType::kPoint;
    light.ambientComp = 0.0f;
    light.diffuseComp = 1.0f;
    light.distance = 8.0f;
    light.coeff = { 1.0f, 0.25f, 0.75f };
    light.color = gfx::Color4(1.0f,1.0f,1.0f,1.0f).toABGR();
    lightNode = _modelStageGraph->createLightNode();
    bx::mtxTranslate(lightNode->transform(), 4.0f, 4.0f, 0.0f);
    lightNode->light()->light = renderService().createLight(light);
    _modelStageGraph->addChildNodeToNode(lightNode, _modelStageGraph->root());
    
    _modelStageCamera.viewIndex = 2;
    _modelStageCamera.near = 0.1f;
    _modelStageCamera.far = 100.f;
    _modelStageCamera.fovDegrees = 60.0f;
    _modelStageCamera.viewportRect = gfx::Rect { 0, 0, 128, 128 };
    _modelStageCamera.worldMtx = gfx::Matrix4::kIdentity;
    _modelStageCamera.update();
    
    bgfx::setViewClear(_modelStageCamera.viewIndex
				, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
				, 0x303030ff
				, 1.0f
				, 0
				);
    
    auto templateManifest = entityService().getDefinitions("entity");
    if (templateManifest && templateManifest->root().HasMember("entity")) {
        auto& templates = templateManifest->root()["entity"];
        for (auto templateIt = templates.MemberBegin();
             templateIt != templates.MemberEnd();
             ++templateIt)
        {
            addEntityTemplateUIData(templateIt->name.GetString(), templateIt->value);
        }
    }
    
    //  reset camera
    cinek::gfx::Matrix4 cameraRotMtx;
    bx::mtxRotateXYZ(cameraRotMtx, 0, 0, 0);
    _freeCameraController.setTransform({ 0,2,-12}, cameraRotMtx);
    
    test1();
    
    //  load scene
    assetService().loadManifest("scenes/apartment.json",
        [this](std::shared_ptr<ove::AssetManifest> manifest) {
            sceneService().initialize(manifest);
            entityService().createEntity(kEntityStore_Default, "entity", "test_bot");
            sceneService().disableSimulation();
            _sceneLoaded = true;
        });
}

void GameView::onViewRemoved(ove::ViewStack& stateController)
{
    _modelStageGraph = nullptr;
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
    if (!_sceneLoaded)
        return;
    
    //  handle keyboard inputs - shortcuts
    _shiftModifierAction = inputState.testKeyMod(KMOD_SHIFT);
    
    if (_shiftModifierAction) {
        if (inputState.testKey(SDL_SCANCODE_A)) {
            _displayTemplateSelector = true;
        }
    }
    
    
    //  layout UI
    uicore::Layout::Style style;
    style.mask = uicore::Layout::Style::has_margins;
    style.margins = { 8, 8, 8, 8 };
    
    uicore::Layout uiLayout;
    auto frameWidth = renderService().getViewRect().w;
    auto frameHeight = renderService().getViewRect().h;
    uiLayout.beginFrame(UI_BUTTON0_DOWN, &_sceneFrameState, viewUIRenderHook, this)
        .setSize(frameWidth, frameHeight);
    
    if (_displayTemplateSelector) {
        uiLayout.beginColumn()
            .setLayout(UI_RIGHT | UI_VCENTER)
            .setSize(frameWidth/5, frameHeight*3/4)
            .beginWindow()
                .listbox(this, kUIProviderId_EntityTemplates,
                    uicore::ListboxType::kList,
                    &_entityTemplateListboxState,
                    &style)
            .end()
        .end();
    }
    
    uiLayout.end();
    
    //  update camera
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
    
    ove::SceneRayTestResult mouseHitTestResult = sceneService().rayTestClosest(
        { pos.x, pos.y, pos.z },
        { dir.x , dir.y, dir.z },
        100.0f);
    
    //  Editor Live Actions based on mouse position and editing state
    updateStagedEntity(mouseHitTestResult);
    
    //  RENDERING
    sceneService().renderDebugStart(renderService(), _camera);
    
    sceneService().renderDebugAddRayTestHit(mouseHitTestResult,
        { pos.x, pos.y, pos.z }, 0.1f, false);
    
    renderService().renderNode(_renderer, sceneService().getGfxRootNode(), _camera);
    
    sceneService().renderDebugEnd();
    
    test2();
    
}

void GameView::onViewEndFrame(ove::ViewStack& stateController)
{
    if (_displayTemplateSelector) {
        if (_entityTemplateListboxState.selected()) {
            _displayTemplateSelector = false;
            
            createAndStageEntity(kEntityStore_Staging,
                "entity",
                _entityTemplateUIList[_entityTemplateListboxState.selectedItem].name);
        }
    }
}

const char* GameView::viewId() const
{
    return "GameView";
}

void GameView::createAndStageEntity
(
    EntityContextType storeId,
    const std::string &ns,
    const std::string &name
)
{
    //  create staging entity with specified template
    //      create entity with the staging store
    _stagedEntity = entityService().createEntity(kEntityStore_Staging, ns, name);
}

void GameView::updateStagedEntity(const ove::SceneRayTestResult& hitResult)
{
    if (!_stagedEntity || !hitResult)
        return;
    
    if (hitResult.entity != _stagedEntity) {
        if (!hitResult.normal.fuzzyZero()) {
            sceneService().setEntityPosition(_stagedEntity,
                hitResult.position,
                hitResult.normal);
        }
    }
}

void GameView::unstageEntity(UnstageOption option)
{
}

////////////////////////////////////////////////////////////////////////

void GameView::viewUIRenderHook(void* context, NVGcontext* nvg)
{
}

void GameView::addEntityTemplateUIData
(
    std::string name,
    const JsonValue& entityTemplate
)
{
    EntityTemplateUIData data;
    data.name = std::move(name);
    
    auto editorTemplate = entityTemplate.FindMember("editor");
    if (editorTemplate != entityTemplate.MemberEnd()) {
        data.longname = editorTemplate->value["name"].GetString();
    }
    
    _entityTemplateUIList.emplace_back(std::move(data));
    
    if (_entityTemplateListboxState.highlightItem < 0) {
        _entityTemplateListboxState.highlightItem = (int)(_entityTemplateUIList.size() - 1);
    }
}

bool GameView::onUIDataItemRequest
(
    int id,
    uint32_t row,
    uint32_t col,
    uicore::DataObject& data
)
{
    if (id == kUIProviderId_EntityTemplates) {
        if (row < _entityTemplateUIList.size()) {
            auto& source = _entityTemplateUIList[row];
            if (col == 0) {
                data.type = uicore::DataObject::Type::string;
                data.data.str = source.longname.empty() ? source.name.c_str()
                                    : source.longname.c_str();
                return true;
            }
        }
    
    }
    
    return false;
}

uint32_t GameView::onUIDataItemRowCountRequest(int id)
{
    if (id == kUIProviderId_EntityTemplates) {
        return (uint32_t)_entityTemplateUIList.size();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////

void GameView::test1()
{
    _testQuadMesh = gfx::createQuad(2.0f, gfx::VertexTypes::kVNormal_Tex0,
        gfx::PrimitiveType::kTriangles);
    
    gfx::MeshBuilder::BuilderState cursorBuilder;
    cursorBuilder.vertexDecl = &gfx::VertexTypes::declaration(gfx::VertexTypes::kVPosition);
    cursorBuilder.indexType = gfx::VertexTypes::Index::kIndex16;
    
    gfx::MeshBuilder::Counts counts =
        gfx::MeshBuilder::calculateUVSphereCounts(7, 14, gfx::PrimitiveType::kLines);
    counts.vertexCount += 2;
    counts.indexCount += 2;
    
    gfx::MeshBuilder::create(cursorBuilder, counts);
    gfx::MeshBuilder::buildUVSphere(cursorBuilder, 0.1f, 7, 14, gfx::PrimitiveType::kLines);
    
    uint16_t i0 = cursorBuilder.vertexCount;
    cursorBuilder.position({ 0,0,0 });
    cursorBuilder.next();
    cursorBuilder.position({ 0,0.5,0 });
    cursorBuilder.next();
    cursorBuilder.line<uint16_t>(i0, i0+1);
    
    _testSphereMesh = std::move(
        gfx::Mesh(gfx::VertexTypes::kVPosition, cursorBuilder.indexType,
            cursorBuilder.vertexMemory,
            cursorBuilder.indexMemory,
            gfx::PrimitiveType::kLines)
    );
}

void GameView::test2()
{
    bgfx::setViewRect(0, _camera.viewportRect.x, _camera.viewportRect.y,
        _camera.viewportRect.w, _camera.viewportRect.h);
    bgfx::setViewTransform(0, _camera.viewMtx, _camera.projMtx);
    
    gfx::Matrix4 mainTransform;
    bx::mtxTranslate(mainTransform, 0.0f, 4.0f, 0.0f);
    
    if (_testSphereMesh.primitiveType() == gfx::PrimitiveType::kTriangles) {
        /*
        bgfx::setTexture(0, renderService().bgfxUniformHandle(gfx::kNodeUniformTexDiffuse),
            _testTarget.texture()->bgfxHandle());
        */
        gfx::Color4 color[2] {
            { 1.0f, 1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f, 1.0f }
        };
        gfx::Vector4 param[2] {
            { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }
        };
        
        bgfx::setUniform(renderService().bgfxUniformHandle(gfx::kNodeUniformColor), color);
        
        bgfx::setUniform(renderService().bgfxUniformHandle(gfx::kNodeUniformMatSpecular), param);
        bgfx::setUniform(renderService().bgfxUniformHandle(gfx::kNodeUniformLightColor), color, 2);

        param[0] = { 0.10f, 0, 0, 0 };
        param[1] = { 0.0f, 1.0f, 0, 0 };
        bgfx::setUniform(renderService().bgfxUniformHandle(gfx::kNodeUniformLightParam), param, 2);

        param[0] = { 0,0,0,0 };
        param[1] = { -1, -1, 0, 0 };
        bx::vec3Norm(param[1], param[1]);
        bgfx::setUniform(renderService().bgfxUniformHandle(gfx::kNodeUniformLightDir), param, 2);
        
        param[0] = { 0,0,0,0 };
        param[1] = { 0,0,0,0 };
        bgfx::setUniform(renderService().bgfxUniformHandle(gfx::kNodeUniformLightOrigin), param, 2);
        bgfx::setUniform(renderService().bgfxUniformHandle(gfx::kNodeUniformLightCoeffs), param, 2);

        bgfx::setTransform(mainTransform);
    
        /*
        bgfx::setVertexBuffer(_testQuadMesh.vertexBuffer());
        bgfx::setIndexBuffer(_testQuadMesh.indexBuffer());
        
        
        bgfx::setState(0
                        | BGFX_STATE_RGB_WRITE
                        | BGFX_STATE_ALPHA_WRITE
                        | BGFX_STATE_DEPTH_WRITE
                        | BGFX_STATE_DEPTH_TEST_LESS
                        | BGFX_STATE_MSAA
                        | BGFX_STATE_CULL_CW
                        );

        bgfx::submit(0, renderService().bgfxProgramHandle(gfx::kNodeProgramMesh));
        */
        
        bgfx::setVertexBuffer(_testSphereMesh.vertexBuffer());
        bgfx::setIndexBuffer(_testSphereMesh.indexBuffer());
        
        bgfx::setState(
            BGFX_STATE_RGB_WRITE
          | BGFX_STATE_ALPHA_WRITE
          | BGFX_STATE_DEPTH_WRITE
          | BGFX_STATE_DEPTH_TEST_LESS
          | BGFX_STATE_MSAA
          | BGFX_STATE_CULL_CW
        );
        
        bgfx::submit(0, renderService().bgfxProgramHandle(gfx::kNodeProgramFlatMesh));
    }
    else if (_testSphereMesh.primitiveType() == gfx::PrimitiveType::kLines) {
        gfx::Color4 color { 1,1,1,1 };
        bgfx::setUniform(renderService().bgfxUniformHandle(gfx::kNodeUniformColor), color);
        
        bgfx::setTransform(mainTransform);
        
        bgfx::setVertexBuffer(_testSphereMesh.vertexBuffer());
        bgfx::setIndexBuffer(_testSphereMesh.indexBuffer());
        
        bgfx::setState(
            BGFX_STATE_RGB_WRITE
          | BGFX_STATE_ALPHA_WRITE
          | BGFX_STATE_DEPTH_WRITE
          | BGFX_STATE_DEPTH_TEST_LESS
          | BGFX_STATE_MSAA
          | BGFX_STATE_PT_LINES
        );
        
        bgfx::submit(0, renderService().bgfxProgramHandle(gfx::kNodeProgramFlat));
    }
}

  /*
    auto renderable = entityTemplate.FindMember("renderable");
    if (renderable == entityTemplate.MemberEnd()) {
        //  TODO - how about editor only displays for non-renderable entities?
        return;
    }
    
    auto modelset = renderService().findModelSet(renderable->value["modelset"].GetString());
    if (!modelset) {
        //  TODO - use a default model
        return;
    }
    
    auto renderTarget = gfx::MultiTextureRenderTarget::createWithDepthTarget(
        _modelStageCamera.viewportRect.w,
        _modelStageCamera.viewportRect.h,
        bgfx::TextureFormat::BGRA8, bgfx::TextureFormat::D16);
    
    CK_ASSERT_RETURN(renderTarget);
    
    auto modelBase = modelset->find(renderable->value["model"].GetString());
    auto modelNode = _modelStageGraph->addChildNodeToNode(_modelStageGraph->clone(modelBase),
                                    _modelStageGraph->root());
    
    //  calculate camera coordinates given the model's aabb
    //  (the model is rendered at world 0,0,0)
    //  (viewport is a square)
    auto modelAABB = modelNode->calculateAABB();
    auto modelDims = modelAABB.dimensions();
    float side = std::max(modelDims.x, modelDims.y) * 0.5f;
    float zdist = side / ckm::tan(_modelStageCamera.fovDegrees*0.5f/180.0f);
    zdist = std::max(zdist, (float)fabs(modelAABB.min.z));
    bx::mtxTranslate(_modelStageCamera.worldMtx,
        modelAABB.min.x + modelDims.x*0.5f,
        modelAABB.min.y + modelDims.y*0.5f,
        -zdist);
    _modelStageCamera.update();
    
    //  take a snapshot of the model, rendered to our target
    renderService().renderNodeToTarget(_renderer,
        (const gfx::RenderTarget&)renderTarget,
        _modelStageGraph->root(),
        _modelStageCamera);
    
    _modelStageGraph->removeNode(modelNode);
    
    //bgfx::TextureFormat::Enum renderTextureFormat = renderTarget.texture()->bgfxFormat();
    bgfx::TextureHandle renderTexture = renderTarget.texture()->release();
    
    int nvImage = nvgCreateImageFromBackendTexture(nvgContext(),
        renderTarget.target().width(), renderTarget.target().height(),
        NVG_TEXTURE_RGBA,
        &renderTexture);
    */


} /* namespace cinek */
