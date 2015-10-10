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

////////////////////////////////////////////////////////////////////////////////

enum
{
    kShaderProgramStdMesh   = 0x00000001
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
    
    programs.fill(BGFX_INVALID_HANDLE);
    programs[cinek::gfx::kNodeProgramMesh] = shaderLibrary.program(kShaderProgramStdMesh);
    uniforms.fill(BGFX_INVALID_HANDLE);
    
    uniforms[cinek::gfx::kNodeUniformTexDiffuse] =
            bgfx::createUniform("u_texColor", bgfx::UniformType::Int1);
    uniforms[cinek::gfx::kNodeUniformMatSpecularColor] =
            bgfx::createUniform("u_specularColor", bgfx::UniformType::Vec4);
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
        
        cinek::gfx::Context gfxContext(gfxInitParams);
        
        
        cinek::AppController appController;
        
        cinek::gfx::NodeGraph model;
        cinek::FileStreamBuf inFile("device.json");
        if (inFile) {
            cinek::RapidJsonStdStreamBuf jsonStreamBuf(inFile);
            cinek::JsonDocument jsonModelDoc;
            jsonModelDoc.ParseStream<0, rapidjson::UTF8<>>(jsonStreamBuf);
        
            model = cinek::gfx::loadNodeGraphFromJSON(gfxContext, jsonModelDoc);
        }
        
        cinek::gfx::NodeRenderer::ProgramMap shaderPrograms;
        cinek::gfx::NodeRenderer::UniformMap shaderUniforms;
        
        registerShaders(shaderLibrary, shaderPrograms, shaderUniforms);
        
        //  create master scene
        cinek::gfx::NodeElementCounts sceneElementCounts;
        sceneElementCounts.nodeCount = 256;
        sceneElementCounts.meshElementCount = 128;
        sceneElementCounts.transformCount = 128;
        
        cinek::gfx::NodeGraph scene(sceneElementCounts);
        auto sceneRoot = scene.createTransformNode();
        bx::mtxIdentity(sceneRoot->transform()->mtx);
        scene.setRoot(sceneRoot);
        
        //  add instances of our model to the master scene
        auto newObjectNode = scene.clone(model.root());
        auto newObjectRoot = scene.createTransformNode();
        bx::mtxTranslate(newObjectRoot->transform()->mtx, 4.0f, 0.0f, 0.0f);
        scene.addChildNodeToNode(newObjectNode, newObjectRoot);
        scene.addChildNodeToNode(newObjectRoot, scene.root());
        
        newObjectNode = scene.clone(model.root());
        newObjectRoot = scene.createTransformNode();
        bx::mtxTranslate(newObjectRoot->transform()->mtx, -4.0f, 0.0f, 0.0f);
        scene.addChildNodeToNode(newObjectNode, newObjectRoot);
        scene.addChildNodeToNode(newObjectRoot, scene.root());
        
        newObjectNode = scene.clone(model.root());
        newObjectRoot = scene.createTransformNode();
        bx::mtxTranslate(newObjectRoot->transform()->mtx, 0.0f, 0.0f, 4.0f);
        scene.addChildNodeToNode(newObjectNode, newObjectRoot);
        scene.addChildNodeToNode(newObjectRoot, scene.root());
        
        newObjectNode = scene.clone(model.root());
        newObjectRoot = scene.createTransformNode();
        bx::mtxTranslate(newObjectRoot->transform()->mtx, 6.0f, 0.0f, 8.5f);
        scene.addChildNodeToNode(newObjectNode, newObjectRoot);
        scene.addChildNodeToNode(newObjectRoot, scene.root());
        
        newObjectNode = scene.clone(model.root());
        newObjectRoot = scene.createTransformNode();
        bx::mtxTranslate(newObjectRoot->transform()->mtx, -3.0f, 0.0f, 11.0f);
        scene.addChildNodeToNode(newObjectNode, newObjectRoot);
        scene.addChildNodeToNode(newObjectRoot, scene.root());

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
