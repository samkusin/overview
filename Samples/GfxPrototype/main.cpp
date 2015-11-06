//
//  Main.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/23/15.
//
//

#include "Common.hpp"
#include "Controller.hpp"

#include "CKGfx/VertexTypes.hpp"
#include "CKGfx/ShaderLibrary.hpp"
#include "CKGfx/Context.hpp"
#include "CKGfx/ModelJsonSerializer.hpp"
#include "CKGfx/NodeGraph.hpp"
#include "CKGfx/NodeRenderer.hpp"
#include "CKGfx/Node.hpp"
#include "CKGfx/AnimationController.hpp"
#include "CKGfx/Light.hpp"
#include "CKGfx/Shaders/ckgfx.sh"

#include <cinek/file.hpp>
#include <cinek/allocator.hpp>
#include <cinek/objectpool.hpp>
#include <cinek/filestreambuf.hpp>
#include <ckjson/jsonstreambuf.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_mouse.h>
#include <bgfx/bgfxplatform.h>
#include <bgfx/bgfx.h>

#include "UICore/UITypes.hpp"
#include "UICore/UIEngine.hpp"
#include "UICore/UIRenderer.hpp"

#include "UICore/oui.h"

#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////

cinek::gfx::NodeGraph loadModelFromFile
(
    cinek::gfx::Context& gfxContext,
    const char* filename
)
{
    cinek::gfx::NodeGraph model;
    cinek::FileStreamBuf inFile(filename);
    if (inFile) {
        cinek::RapidJsonStdStreamBuf jsonStreamBuf(inFile);
        cinek::JsonDocument jsonModelDoc;
        jsonModelDoc.ParseStream<0, rapidjson::UTF8<>>(jsonStreamBuf);
    
        model = std::move(cinek::gfx::loadNodeGraphFromJSON(gfxContext, jsonModelDoc));
    }
    return model;
}

enum
{
    kShaderProgramStdMesh   = 0x00000001,
    kShaderProgramBoneMesh  = 0x00000002
};

static void registerShaders
(
    cinek::gfx::ShaderLibrary& shaderLibrary,
    cinek::gfx::NodeRenderer::ProgramMap& programs,
    cinek::gfx::NodeRenderer::UniformMap& uniforms
)
{
    shaderLibrary.loadProgram(kShaderProgramStdMesh,
            "bin/vs_std_uv.bin",
            "bin/fs_std_tex.bin");
    
    shaderLibrary.loadProgram(kShaderProgramBoneMesh,
            "bin/vs_bone_uv.bin",
            "bin/fs_std_tex.bin");
    
    programs.fill(BGFX_INVALID_HANDLE);
    programs[cinek::gfx::kNodeProgramMesh] = shaderLibrary.program(kShaderProgramStdMesh);
    programs[cinek::gfx::kNodeProgramBoneMesh] = shaderLibrary.program(kShaderProgramBoneMesh);
  
    uniforms.fill(BGFX_INVALID_HANDLE);
    uniforms[cinek::gfx::kNodeUniformWorldMtx] =
            bgfx::createUniform("u_world", bgfx::UniformType::Mat4);
    uniforms[cinek::gfx::kNodeUniformWorldViewProjMtx] =
            bgfx::createUniform("u_worldViewProj", bgfx::UniformType::Mat4);
    
    uniforms[cinek::gfx::kNodeUniformTexDiffuse] = 
            bgfx::createUniform("u_texColor", bgfx::UniformType::Int1);
    uniforms[cinek::gfx::kNodeUniformMatSpecular] =
            bgfx::createUniform("u_specularity", bgfx::UniformType::Vec4);
    
    uniforms[cinek::gfx::kNodeUniformLightColor] =
            bgfx::createUniform("u_lightColor", bgfx::UniformType::Vec4,
                                CKGFX_SHADERS_LIGHT_COUNT);
    uniforms[cinek::gfx::kNodeUniformLightParam] =
            bgfx::createUniform("u_lightParam", bgfx::UniformType::Vec4,
                                CKGFX_SHADERS_LIGHT_COUNT);
    uniforms[cinek::gfx::kNodeUniformLightDir] =
            bgfx::createUniform("u_lightDir", bgfx::UniformType::Vec4,
                                CKGFX_SHADERS_LIGHT_COUNT);
    uniforms[cinek::gfx::kNodeUniformLightCoeffs] =
            bgfx::createUniform("u_lightCoeff", bgfx::UniformType::Vec4,
                                CKGFX_SHADERS_LIGHT_COUNT);
    uniforms[cinek::gfx::kNodeUniformLightOrigin] =
            bgfx::createUniform("u_lightOrigin", bgfx::UniformType::Vec4,
                                CKGFX_SHADERS_LIGHT_COUNT);
}


////////////////////////////////////////////////////////////////////////////////


static uint32_t pollSDLEvents(PollStateSDL& state)
{
    int mx, my;
    
    //  handle Mouse UI, which is polled per frame instead of set per event.
    uint32_t mbtn = SDL_GetMouseState(&mx, &my);
    SDL_GetRelativeMouseState(&state.mdx, &state.mdy);
    
    uiSetCursor(mx, my);
    
    uiSetButton(0, 0, (mbtn & SDL_BUTTON_LMASK) != 0);
    uiSetButton(2, 0, (mbtn & SDL_BUTTON_RMASK) != 0);
    
    state.mx = mx;
    state.my = my;
    state.mbtn = mbtn;
    state.mxWheel = 0;
    state.myWheel = 0;
    
    //  poll system and key events
    uint32_t flags = 0;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            flags |= kPollSDLEvent_Quit;
            break;
        }
        else if (event.type == SDL_MOUSEWHEEL)
        {
            if (event.wheel.which != SDL_TOUCH_MOUSEID)
            {
                state.mxWheel += event.wheel.x;
                state.myWheel += event.wheel.y;
                uiSetScroll(state.mxWheel, state.myWheel);
            }
        }
        else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        {
            SDL_KeyboardEvent& kevt = event.key;
            if (!kevt.repeat)
            {
                uiSetKey(kevt.keysym.scancode, kevt.keysym.mod, kevt.state);
            }
        }
        else if (event.type == SDL_TEXTINPUT)
        {
            uiSetChar(event.text.text[0]);
        }
    }
    
    state.keystate = SDL_GetKeyboardState(&state.keystateArraySize);
    
    
    
    return flags;
}

static int run(SDL_Window* window)
{
    int viewWidth, viewHeight;
    
    SDL_GetWindowSize(window, &viewWidth, &viewHeight);

    bgfx::reset(viewWidth, viewHeight, BGFX_RESET_VSYNC);
    bgfx::setDebug(BGFX_DEBUG_TEXT /*|BGFX_DEBUG_STATS*/ );
    
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        0x001122ff,
        1.0f,
        0);
    
    //  UI
    UIcontext* uiContext = uiCreateContext(4096, 1<<20);
    uiMakeCurrent(uiContext);
    uiSetHandler(cinek::uicore::OUIHandler);
    
    NVGcontext* nvg = cinek::uicore::createRenderingContext(1);
    if (!nvg)
        return 1;
    {
        //  GFX
        cinek::Allocator allocator;
        cinek::gfx::Rect viewRect = { 0, 0, viewWidth, viewHeight };
        cinek::gfx::VertexTypes::initialize();
        cinek::gfx::ShaderLibrary shaderLibrary;
        
        cinek::gfx::Context::ResourceInitParams gfxInitParams;
        gfxInitParams.numMeshes = 1024;
        gfxInitParams.numMaterials = 1024;
        gfxInitParams.numTextures = 256;
        gfxInitParams.numAnimations = 256;
        gfxInitParams.numLights = 64;
        
        cinek::gfx::Context gfxContext(gfxInitParams);
        
        cinek::AppController appController;
        
        cinek::gfx::NodeGraph testScene = loadModelFromFile(gfxContext, "scene.json");
        cinek::gfx::NodeGraph model = loadModelFromFile(gfxContext, "device.json");
        cinek::gfx::NodeGraph robot = loadModelFromFile(gfxContext, "verybadrobot.json");
        cinek::gfx::NodeGraph factorybot = loadModelFromFile(gfxContext, "simple.json");
        
        cinek::gfx::NodeRenderer::ProgramMap shaderPrograms;
        cinek::gfx::NodeRenderer::UniformMap shaderUniforms;
        
        registerShaders(shaderLibrary, shaderPrograms, shaderUniforms);
        
        cinek::gfx::AnimationControllerPool animControllerPool(256);
        
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
        
        /*
        auto root = scene.createObjectNode(0);
        bx::mtxIdentity(root->transform());
        scene.setRoot(root);
        */

        scene.setRoot(testScene.root());
        auto sceneRoot = scene.root();
        
        /*
        auto lightRoot = scene.createObjectNode(0);
        bx::mtxIdentity(lightRoot->transform());
        
        //  directional
        auto lightNode = scene.createLightNode();
        cinek::gfx::Matrix4 mtx;
        cinek::gfx::Vector3 dir { -0.2f, -0.75f, 0.55f };
        bx::mtxLookAt(mtx, dir, cinek::gfx::Vector3::kZero);
        bx::mtxInverse(lightNode->transform(), mtx);
        
        cinek::gfx::Light light;
        light.type = cinek::gfx::LightType::kDirectional;
        light.ambientComp = 0.25f;
        light.diffuseComp = 0.25f;
        light.color = cinek::gfx::Color4(0.9f, 0.9f, 1.0f, 1.0f).toABGR();
        light.distance = 0.0f;
        light.cutoff = 0.0f;
        lightNode->light()->light = gfxContext.registerLight(std::move(light));
        scene.addChildNodeToNode(lightNode, lightRoot);
        
        //  point light
        lightNode = scene.createLightNode();
        bx::mtxTranslate(lightNode->transform(), 10.0f, 10.0f, -2.0f);
        
        light.type = cinek::gfx::LightType::kPoint;
        light.ambientComp = 0.0f;
        light.diffuseComp = 1.0f;
        light.distance = 100.0f;
        light.cutoff = 0.0f;
        light.color = cinek::gfx::Color4(1.0f, 1.0f, 1.0f, 1.0f).toABGR();
        light.coeff = cinek::gfx::Vector3(1.0f, 0.033f, 0.00075f);
        lightNode->light()->light = gfxContext.registerLight(std::move(light));
        scene.addChildNodeToNode(lightNode, lightRoot);
        
        //  spot light
        lightNode = scene.createLightNode();
        cinek::gfx::Vector3 spotSrc { -12.0f, 1.0f, 10.0f };
        dir = { -1.0f, 0.0, 1.0f };
        dir = spotSrc + dir;
        bx::mtxLookAt(mtx, spotSrc, dir);
        bx::mtxInverse(lightNode->transform(), mtx);
        
        light.type = cinek::gfx::LightType::kSpot;
        light.ambientComp = 0.0f;
        light.diffuseComp = 1.0f;
        light.distance = 100.0f;
        light.cutoff = 0.2f;
        light.color = cinek::gfx::Color4(0.0f, 1.0f, 1.0f, 1.0f).toABGR();
        light.coeff = cinek::gfx::Vector3(1.0f, 0.025f, 0.025f);
        lightNode->light()->light = gfxContext.registerLight(std::move(light));
        scene.addChildNodeToNode(lightNode, lightRoot);
        
        scene.addChildNodeToNode(lightRoot, sceneRoot);
        */
        
        cinek::gfx::NodeHandle newObjectNode;
        cinek::gfx::NodeHandle newObjectRoot;
 
 /*
        //  add instances of our model to the master scene
        //  generic building, sculpture
        newObjectNode = scene.clone(model.root());
        newObjectRoot = scene.createObjectNode(0);
        bx::mtxSRT(newObjectRoot->transform(),
            1,1,1,
            0, 0, 0,
            0, 0, 0);
        scene.addChildNodeToNode(newObjectNode, newObjectRoot);
        scene.addChildNodeToNode(newObjectRoot, scene.root());
        
        newObjectNode = scene.clone(model.root());
        newObjectRoot = scene.createObjectNode(0);
        bx::mtxSRT(newObjectRoot->transform(),
            1,1,1,
            0, 0, 0,
            -4, 0, -1);
        scene.addChildNodeToNode(newObjectNode, newObjectRoot);
        scene.addChildNodeToNode(newObjectRoot, scene.root());
        
        newObjectNode = scene.clone(model.root());
        newObjectRoot = scene.createObjectNode(0);
        bx::mtxSRT(newObjectRoot->transform(),
            1,1,1,
            0, 0, 0,
            4, 0, -1);
        scene.addChildNodeToNode(newObjectNode, newObjectRoot);
        scene.addChildNodeToNode(newObjectRoot, scene.root());
        
        newObjectNode = scene.clone(model.root());
        newObjectRoot = scene.createObjectNode(0);
        bx::mtxSRT(newObjectRoot->transform(),
            1,1,1,
            0, 0, 0,
            2, 0, 4);
        scene.addChildNodeToNode(newObjectNode, newObjectRoot);
        scene.addChildNodeToNode(newObjectRoot, scene.root());
        
        newObjectNode = scene.clone(model.root());
        newObjectRoot = scene.createObjectNode(0);
        bx::mtxSRT(newObjectRoot->transform(),
            1,1,1,
            0, 0, 0,
            -3, 0, 7);
        scene.addChildNodeToNode(newObjectNode, newObjectRoot);
        scene.addChildNodeToNode(newObjectRoot, scene.root());
*/

        //  robot-cyborg
        newObjectNode = scene.clone(robot.root());
        newObjectRoot = scene.createObjectNode(0);
        bx::mtxSRT(newObjectRoot->transform(),
            1.25f, 1.25f, 1.25f,
            0, 0, 0,
            -1, 0, -3);
        scene.addChildNodeToNode(newObjectNode, newObjectRoot);
        scene.addChildNodeToNode(newObjectRoot, scene.root());

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

        //  factorybot
        newObjectNode = scene.clone(factorybot.root());
        newObjectRoot = scene.createObjectNode(101);
        bx::mtxSRT(newObjectRoot->transform(),
            1.0f, 1.0f, 1.0f,
            0, 0, 0,
            3.5, 0, -3.5);
        scene.addChildNodeToNode(newObjectNode, newObjectRoot);
        scene.addChildNodeToNode(newObjectRoot, scene.root());
        
        nodeVisitCtx.nodeId = newObjectRoot->objectNodeId();
        cinek::gfx::visit(newObjectNode, [&nodeVisitCtx](cinek::gfx::NodeHandle node) -> bool {
            if (node->elementType() == cinek::gfx::Node::kElementTypeArmature) {
                cinek::gfx::AnimationController controller(node->armature()->animSet);
                auto animController = nodeVisitCtx.ctrlPool->add(std::move(controller));
                node->armature()->animController = animController;
                nodeVisitCtx.ctrlMap->emplace(nodeVisitCtx.nodeId, animController);
                animController->transitionToState("idle");
            }
            return true;
        });

        //  Renderer initialization
        cinek::gfx::NodeRenderer nodeRenderer(shaderPrograms, shaderUniforms);
        cinek::gfx::Camera mainCamera;
        mainCamera.viewFrustrum = cinek::gfx::Frustrum(0.1, 100.0, M_PI * 60/180.0f,
            (float)viewWidth/viewHeight);
        
        uint32_t systemTimeMs = SDL_GetTicks();
        //uint32_t testTimeMs = 0;
        bool running = true;
        
        while (running) {
            uint32_t nextSystemTimeMs = SDL_GetTicks();
            int32_t frameTimeMs = nextSystemTimeMs - systemTimeMs;
            systemTimeMs = nextSystemTimeMs;

            /*
            if (frameTimeMs > 50) {
                testTimeMs += 50;
            }
            else {
                testTimeMs += 2;
            }
            */

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
                appController.handleCameraInput(mainCamera, polledInputState, frameTimeMs*.001f);
                uiProcess(systemTimeMs);
            }

            bgfx::frame();
        }
    }
    
    cinek::uicore::destroyRenderingContext(nvg);
    
    return 0;
}


int OverviewMain(SDL_Window* window, int argc, char* argv[])
{
    cinek::file::setOpsStdio();
    
    //  renderer initialization
    //
    bgfx::sdlSetWindow(window);
    bgfx::init();
    
    int result = run(window);
    
    //  subsystem termination
    //
    bgfx::shutdown();
    
    return result;
}
