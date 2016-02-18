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

#include <SDL2/SDL_timer.h>
#include <bgfx/bgfx.h>

#include "UICore/UITypes.hpp"
#include "UICore/UIEngine.hpp"
#include "UICore/UIRenderer.hpp"
#include "UICore/Input.hpp"

#include "UICore/oui.h"

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

int runSample(int viewWidth, int viewHeight)
{
    //  UI
    UIcontext* uiContext = uiCreateContext(4096, 1<<20);
    uiMakeCurrent(uiContext);
    uiSetHandler(cinek::uicore::OUIHandler);
    
    NVGcontext* nvg = cinek::uicore::createRenderingContext(1);
    if (!nvg)
        return 1;
    
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
        const double kSecsPerSimFrame = 1/kSimFPS;
        
        double simTime = 0.0;
        double lagSecsSim = 0.0;
        
        uint32_t systemTimeMs = SDL_GetTicks();
        bool running = true;
        
        cinek::uicore::InputState polledInputState;
        
        while (running) {
            uint32_t nextSystemTimeMs = SDL_GetTicks();
            int32_t frameTimeMs = nextSystemTimeMs - systemTimeMs;
            systemTimeMs = nextSystemTimeMs;
            
            controller.beginFrame();
            
            //  TODO: Lag should not be incremented while Sim is Paused
            double frameTime = frameTimeMs*0.001;
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

            if (cinek::uicore::pollSDLEvents(polledInputState) & cinek::uicore::kPollSDLEvent_Quit)
                running = false;
            

            controller.renderFrame(frameTime, viewRect, polledInputState);

            cinek::uicore::render(nvg, viewRect);
        
            uiProcess(systemTimeMs);
            
            controller.endFrame();
            
            bgfx::frame();
        }
    }
    
    cinek::uicore::destroyRenderingContext(nvg);
    
    return 0;
}



