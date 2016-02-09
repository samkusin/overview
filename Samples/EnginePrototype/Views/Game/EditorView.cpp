//
//  EditorView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/15/16.
//
//

#include "EditorView.hpp"
#include "EditorMain.hpp"
#include "EditorAddEntityToScene.hpp"

#include "Engine/AssetManifest.hpp"
#include "Engine/Services/SceneService.hpp"
#include "Engine/Services/RenderService.hpp"

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

EditorView::EditorView
(
    const GameViewContext* gameContext
) :
    _gc(gameContext)
{
    _mainState = std::allocate_shared<EditorMain>(std_allocator<EditorMain>(), gameContext);
    _addEntityToSceneState = std::allocate_shared<EditorAddEntityToScene>(
        std_allocator<EditorAddEntityToScene>(),
        _gc);
}

    
void EditorView::onViewAdded(ove::ViewStack& stateController)
{
    _viewStack.setFactory(
        [this](const std::string& viewName, ove::ViewController* )
            -> std::shared_ptr<ove::ViewController> {
            //  Startup View initialzes common data for the application
            //  Game View encompasses the whole game simulation
            //  Ship View encompasses the in-game ship mode
            //  Ship Bridge View encompasses the in-game ship bridge mode
            if (viewName == "EditorMain") {
                return _mainState;
            }
            else if (viewName == "EditorAddEntityToScene") {
                return _addEntityToSceneState;
            }
            
            return nullptr;
        });
    
    _viewStack.present("EditorMain");


    /*
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
    */
    
    
    //  reset camera
    cinek::gfx::Matrix4 cameraRotMtx;
    bx::mtxRotateXYZ(cameraRotMtx, 0, 0, 0);
    _freeCameraController.setTransform({ 0,2,-12}, cameraRotMtx);
    
    test1();
    
    //  load scene
    
    _gc->sceneService->disableSimulation();
}

void EditorView::onViewRemoved(ove::ViewStack& stateController)
{
    _viewStack.pop();
}

void EditorView::onViewStartFrame(ove::ViewStack& stateController)
{
    _viewStack.startFrame();
}

void EditorView::simulateView(ove::ViewStack& stateController, double dt)
{
    _viewStack.simulate(dt);
}

void EditorView::frameUpdateView
(
    ove::ViewStack& stateController,
    double dt,
    const cinek::uicore::InputState& inputState
)
{
    test2();
    
    _freeCameraController.handleCameraInput(*_gc->camera, inputState, dt);
    
    _viewStack.frameUpdate(dt, inputState);
}

void EditorView::onViewEndFrame(ove::ViewStack& stateController)
{
    _viewStack.endFrame();
}

const char* EditorView::viewId() const
{
    return "EditorView";
}


////////////////////////////////////////////////////////////////////////

bool EditorView::onUIDataItemRequest
(
    int id,
    uint32_t row,
    uint32_t col,
    uicore::DataObject& data
)
{
     return false;
}

uint32_t EditorView::onUIDataItemRowCountRequest(int id)
{
    return 0;
}

////////////////////////////////////////////////////////////////////////

void EditorView::test1()
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

void EditorView::test2()
{
    auto& camera = *_gc->camera;
    auto& renderService = *_gc->renderService;
    
    bgfx::setViewRect(0, camera.viewportRect.x, camera.viewportRect.y,
        camera.viewportRect.w, camera.viewportRect.h);
    bgfx::setViewTransform(0, camera.viewMtx, camera.projMtx);
    
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

        bgfx::setUniform(renderService.bgfxUniformHandle(gfx::kNodeUniformColor), color);
        
        bgfx::setUniform(renderService.bgfxUniformHandle(gfx::kNodeUniformMatSpecular), param);
        bgfx::setUniform(renderService.bgfxUniformHandle(gfx::kNodeUniformLightColor), color, 2);

        param[0] = { 0.10f, 0, 0, 0 };
        param[1] = { 0.0f, 1.0f, 0, 0 };
        bgfx::setUniform(renderService.bgfxUniformHandle(gfx::kNodeUniformLightParam), param, 2);

        param[0] = { 0,0,0,0 };
        param[1] = { -1, -1, 0, 0 };
        bx::vec3Norm(param[1], param[1]);
        bgfx::setUniform(renderService.bgfxUniformHandle(gfx::kNodeUniformLightDir), param, 2);
        
        param[0] = { 0,0,0,0 };
        param[1] = { 0,0,0,0 };
        bgfx::setUniform(renderService.bgfxUniformHandle(gfx::kNodeUniformLightOrigin), param, 2);
        bgfx::setUniform(renderService.bgfxUniformHandle(gfx::kNodeUniformLightCoeffs), param, 2);

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
        
        bgfx::submit(0, renderService.bgfxProgramHandle(gfx::kNodeProgramMeshColor));
    }
    else if (_testSphereMesh.primitiveType() == gfx::PrimitiveType::kLines) {
        gfx::Color4 color { 1,1,1,1 };
        bgfx::setUniform(renderService.bgfxUniformHandle(gfx::kNodeUniformColor), color);
        
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
        
        bgfx::submit(0, renderService.bgfxProgramHandle(gfx::kNodeProgramFlat));
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
