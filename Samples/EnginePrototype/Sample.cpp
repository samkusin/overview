//
//  Main.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/23/15.
//
//

#include "Common.hpp"
#include "Renderer.hpp"

#include "CKGfx/VertexTypes.hpp"
#include "CKGfx/ShaderLibrary.hpp"
#include "CKGfx/Context.hpp"
#include "CKGfx/ModelJsonSerializer.hpp"
#include "CKGfx/NodeGraph.hpp"
#include "CKGfx/NodeRenderer.hpp"
#include "CKGfx/Node.hpp"
#include "CKGfx/AnimationController.hpp"
#include "CKGfx/Light.hpp"

#include <cinek/file.hpp>
#include <cinek/allocator.hpp>
#include <cinek/objectpool.hpp>
#include <cinek/filestreambuf.hpp>
#include <ckjson/jsonstreambuf.hpp>

#include <SDL2/SDL_timer.h>
#include <bgfx/bgfx.h>

#include "UICore/UITypes.hpp"
#include "UICore/UIEngine.hpp"
#include "UICore/UIRenderer.hpp"

#include "UICore/oui.h"

#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////

enum
{
    kShaderProgramStdMesh   = 0x00000001,
    kShaderProgramBoneMesh  = 0x00000002
};


int runSample(int viewWidth, int viewHeight)
{
    //  UI
    UIcontext* uiContext = uiCreateContext(4096, 1<<20);
    uiMakeCurrent(uiContext);
    uiSetHandler(cinek::uicore::OUIHandler);
    
    NVGcontext* nvg = cinek::uicore::createRenderingContext(1);
    if (!nvg)
        return 1;
    
    //  GFX
    cinek::Allocator allocator;
    cinek::gfx::Rect viewRect = { 0, 0, viewWidth, viewHeight };
    cinek::gfx::VertexTypes::initialize();
    cinek::gfx::ShaderLibrary shaderLibrary;
    cinek::gfx::NodeRenderer::ProgramMap shaderPrograms;
    cinek::gfx::NodeRenderer::UniformMap shaderUniforms;
    
    registerShaders(
        shaderLibrary, shaderPrograms, shaderUniforms, {
            {
                cinek::gfx::kNodeProgramMesh, kShaderProgramStdMesh,
                "bin/vs_std_uv.bin",
                "bin/fs_std_tex.bin"
            },
            {
                cinek::gfx::kNodeProgramBoneMesh,kShaderProgramBoneMesh,
                "bin/vs_bone_uv.bin",
                "bin/fs_std_tex.bin"
            }
        });

    cinek::gfx::Context::ResourceInitParams gfxInitParams;
    
    gfxInitParams.numMeshes = 1024;
    gfxInitParams.numMaterials = 1024;
    gfxInitParams.numTextures = 256;
    gfxInitParams.numAnimations = 256;
    gfxInitParams.numLights = 64;
    
    cinek::gfx::Context gfxContext(gfxInitParams);
    cinek::gfx::AnimationControllerPool animControllerPool(256);
    
    //  Application
    cinek::gfx::NodeGraph testScene = loadModelFromFile(gfxContext, "scene.json");
    
    
    std::unordered_map<cinek::gfx::NodeId,
                       cinek::gfx::AnimationControllerHandle> animControllers;
    
    //  create master scene
    cinek::gfx::NodeElementCounts sceneElementCounts;
    sceneElementCounts.meshNodeCount = 128;
    sceneElementCounts.armatureNodeCount = 32;
    sceneElementCounts.lightNodeCount = 8;
    sceneElementCounts.objectNodeCount = 64;
    sceneElementCounts.transformNodeCount = 64;
    
    cinek::gfx::NodeGraph scene(sceneElementCounts);
    scene.setRoot(testScene.root());

    /*
    auto sceneRoot = scene.root();
    cinek::gfx::NodeHandle newObjectNode;
    cinek::gfx::NodeHandle newObjectRoot;

    struct GfxNodeVisitContext
    {
        cinek::gfx::NodeId nodeId;
        cinek::gfx::AnimationControllerPool* ctrlPool;
        std::unordered_map<cinek::gfx::NodeId, cinek::gfx::AnimationControllerHandle>* ctrlMap;
    };

    newObjectNode = scene.clone(factorybot.root());
    newObjectRoot = scene.createObjectNode(100);
    bx::mtxSRT(newObjectRoot->transform(),
        1.0f, 1.0f, 1.0f,
        0, 0, 0,
        0, 0, -5);
    scene.addChildNodeToNode(newObjectNode, newObjectRoot);
    scene.addChildNodeToNode(newObjectRoot, scene.root());
    
    GfxNodeVisitContext nodeVisitCtx {
        newObjectRoot->objectNodeId(),
        &animControllerPool,
        &animControllers
    };
    
    cinek::gfx::visit(newObjectNode, [&nodeVisitCtx](cinek::gfx::NodeHandle node) -> bool {
        if (node->elementType() == cinek::gfx::Node::kElementTypeArmature) {
            cinek::gfx::AnimationController controller(node->armature()->animSet);
            auto animController = nodeVisitCtx.ctrlPool->add(std::move(controller));
            node->armature()->animController = animController;
            nodeVisitCtx.ctrlMap->emplace(nodeVisitCtx.nodeId, animController);
            animController->transitionToState("aggro");
        }
        return true;
    });
    */

    //  Renderer initialization
    cinek::gfx::NodeRenderer nodeRenderer(shaderPrograms, shaderUniforms);
    cinek::gfx::Camera mainCamera;
    mainCamera.viewFrustrum = cinek::gfx::Frustrum(0.1, 100.0, M_PI * 60/180.0f,
        (float)viewWidth/viewHeight);
    
    uint32_t systemTimeMs = SDL_GetTicks();
    bool running = true;
    
    while (running) {
        uint32_t nextSystemTimeMs = SDL_GetTicks();
        int32_t frameTimeMs = nextSystemTimeMs - systemTimeMs;
        systemTimeMs = nextSystemTimeMs;

        PollStateSDL polledInputState;
        
        if (pollSDLEvents(polledInputState) & kPollSDLEvent_Quit)
            running = false;

        {
            for (auto& animController : animControllers) {
                animController.second->update(systemTimeMs * 0.001);
            }
        
            gfxContext.update();
            
            bgfx::setViewRect(0, viewRect.x, viewRect.y, viewRect.w, viewRect.h);
            
            nodeRenderer.setCamera(mainCamera);
            nodeRenderer(scene.root());

            cinek::uicore::render(nvg, viewRect);
        }
        {
            //appController.handleCameraInput(mainCamera, polledInputState, frameTimeMs*.001f);
            uiProcess(systemTimeMs);
        }

        bgfx::frame();
    }

    cinek::uicore::destroyRenderingContext(nvg);
    
    return 0;
}



