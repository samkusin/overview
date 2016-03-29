//
//  Main.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 9/23/15.
//
//

#include "Common.hpp"
#include "Renderer.hpp"
#include "Input.hpp"

#include "CKGfx/VertexTypes.hpp"
#include "CKGfx/ShaderLibrary.hpp"
#include "CKGfx/Context.hpp"
#include "CKGfx/ModelJsonSerializer.hpp"
#include "CKGfx/NodeGraph.hpp"
#include "CKGfx/NodeRenderer.hpp"
#include "CKGfx/Node.hpp"
#include "CKGfx/AnimationController.hpp"
#include "CKGfx/Light.hpp"
#include "CKGfx/External/nanovg/nanovg.h"

#include <cinek/file.hpp>
#include <cinek/allocator.hpp>
#include <cinek/objectpool.hpp>

#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_mouse.h>

#include <bgfx/bgfx.h>

#include "UICore/UIEngine.hpp"

#include <unordered_map>

#include "Engine/EngineTypes.hpp"

#include "PrototypeApplication.hpp"



////////////////////////////////////////////////////////////////////////////////
//
//  Main
//
////////////////////////////////////////////////////////////////////////////////

enum
{
    kShaderProgramStdMesh       = 0x00000001,
    kShaderProgramBoneMesh      = 0x00000002,
    kShaderProgramBoneColorMesh = 0x00000003,
    kShaderProgramFlat          = 0x00000004,
    kShaderProgramColorMesh     = 0x00000005,
    kShaderProgramColor         = 0x00000006,
    kShaderProgramDiffuse       = 0x00000007
};

NVGcontext* createNVGcontext(int viewId)
{
    NVGcontext* nvg = nvgCreate(1, viewId);
    bgfx::setViewSeq(viewId, true);
    
    return nvg;
}


int runSample(int viewWidth, int viewHeight, int firstFreeViewId)
{
    NVGcontext* nvg = nvgCreate(1, firstFreeViewId);
    bgfx::setViewSeq(1, true);
    ++firstFreeViewId;
    
    //  nvg destruction should occur after the below scope's objects have been
    //  lost
    {
        //  GFX
        cinek::gfx::Rect viewRect = { 0, 0, viewWidth, viewHeight };
        cinek::gfx::VertexTypes::initialize();
        cinek::gfx::ShaderLibrary shaderLibrary;
        cinek::gfx::NodeRenderer::ProgramMap shaderPrograms;
        cinek::gfx::NodeRenderer::UniformMap shaderUniforms;
        
        registerShaders(
            shaderLibrary, shaderPrograms, shaderUniforms, {
                {
                    cinek::gfx::kNodeProgramMeshUV, kShaderProgramStdMesh,
                    "bin/vs_std_uv.bin",
                    "bin/fs_std_tex.bin"
                },
                {
                    cinek::gfx::kNodeProgramBoneMeshUV, kShaderProgramBoneMesh,
                    "bin/vs_bone_uv.bin",
                    "bin/fs_std_tex.bin"
                },
                {
                    cinek::gfx::kNodeProgramBoneMeshColor, kShaderProgramBoneColorMesh,
                    "bin/vs_bone_col.bin",
                    "bin/fs_std_col.bin"
                },
                {
                    cinek::gfx::kNodeProgramFlat, kShaderProgramFlat,
                    "bin/vs_flat_pos.bin",
                    "bin/fs_flat_col.bin"
                },
                {
                    cinek::gfx::kNodeProgramMeshColor, kShaderProgramColorMesh,
                    "bin/vs_std_flat.bin",
                    "bin/fs_std_col.bin"
                },
                {
                    cinek::gfx::kNodeProgramColor, kShaderProgramColor,
                    "bin/vs_basic_col.bin",
                    "bin/fs_flat_col.bin"
                },
                {
                    cinek::gfx::kNodeProgramDiffuse, kShaderProgramDiffuse,
                    "bin/vs_basic_uv.bin",
                    "bin/fs_flat_tex.bin"
                }
            });

        cinek::gfx::Context::ResourceInitParams gfxInitParams;
        
        gfxInitParams.numMeshes = 1024;
        gfxInitParams.numMaterials = 1024;
        gfxInitParams.numTextures = 256;
        gfxInitParams.numAnimations = 256;
        gfxInitParams.numLights = 64;
        gfxInitParams.numModelSets = 64;
        
        cinek::gfx::Context gfxContext(gfxInitParams);
        
        //  Application
        //
        cinek::PrototypeApplication controller(gfxContext, shaderPrograms, shaderUniforms,
                                               nvg);

        const double kSimFPS = 60.0;
        const CKTimeDelta kSecsPerSimFrame = 1/kSimFPS;
        
        CKTime simTime = 0.0;
        CKTime lagSecsSim = 0.0;
        
        uint32_t systemTimeMs = SDL_GetTicks();
        bool running = true;
        
        cinek::input::InputState polledInputState;
        
        while (running) {
            uint32_t nextSystemTimeMs = SDL_GetTicks();
            int32_t frameTimeMs = nextSystemTimeMs - systemTimeMs;
            systemTimeMs = nextSystemTimeMs;
            
            controller.beginFrame();
            
            //  TODO: Lag should not be incremented while Sim is Paused
            if (frameTimeMs > 100) {
                frameTimeMs = 100;
            }
            
            CKTimeDelta frameTime = frameTimeMs*0.001;
            lagSecsSim += frameTime;
        
            ////////////////////////////////////////////////////////////////////////
            //  SIMULATION START (using a fixed timestep)
            //      All subsystems driven by the application simulation framerate.
            //
            while (lagSecsSim >= kSecsPerSimFrame)
            {
                controller.simulateFrame(kSecsPerSimFrame);

                lagSecsSim -= kSecsPerSimFrame;
                simTime += kSecsPerSimFrame;
                
                //  diagnostics.incrementRateGauge(Diagnostics::kFrameRate_Update);
            }
            //
            //  SIMULATION END
            ////////////////////////////////////////////////////////////////////////

            {
                int mx, my;
    
                //  handle Mouse UI, which is polled per frame instead of set per event.
                uint32_t mbtn = SDL_GetMouseState(&mx, &my);
                SDL_GetRelativeMouseState(&polledInputState.mdx, &polledInputState.mdy);
    
                polledInputState.mx = mx;
                polledInputState.my = my;
                polledInputState.mbtn = mbtn;
                polledInputState.mxWheel = 0;
                polledInputState.myWheel = 0;
    
                //  poll system and key events
                uint32_t flags = 0;
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    flags |= cinek::input::processSDLEvent(polledInputState, event);
                    imGuiProcessEvent(&event);
                }
                if (flags & cinek::input::kPollSDLEvent_Quit)
                    running = false;
                
                polledInputState.keystate = SDL_GetKeyboardState(&polledInputState.keystateArraySize);
                polledInputState.keyModifiers = SDL_GetModState();
            }
            
            //  pixel ratio may change if for some reason we have different framebuffer
            //  sizes from our window (i.e. fullscreen?)
            nvgBeginFrame(nvg, viewRect.w, viewRect.h, 1.0f);
            
            imGuiNewFrame();
            
            controller.renderFrame(frameTime, viewRect, polledInputState);
            
            imGuiRender();
        
            nvgEndFrame(nvg);
        
            controller.endFrame();
            
            bgfx::frame();
        }
    }
    
    nvgDelete(nvg);
    
    return 0;
}



