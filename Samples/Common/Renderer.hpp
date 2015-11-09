//
//  Renderer.hpp
//  SampleCommon
//
//  Created by Samir Sinha on 11/8/15.
//  Copyright © 2015 Cinekine. All rights reserved.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include "CKGfx/NodeRenderer.hpp"

#include <vector>

struct SampleShaderProgram
{
    cinek::gfx::NodeProgramSlot programSlot;
    cinek::gfx::ShaderProgramId programId;
    const char* vsBinaryPath;
    const char* fsBinaryPath;
};

void registerShaders
(
    cinek::gfx::ShaderLibrary& shaderLibrary,
    cinek::gfx::NodeRenderer::ProgramMap& programs,
    cinek::gfx::NodeRenderer::UniformMap& uniforms,
    const std::vector<SampleShaderProgram>& configs
);

cinek::gfx::NodeGraph loadModelFromFile
(
    cinek::gfx::Context& gfxContext,
    const char* filename
);

#endif /* Renderer_hpp */
