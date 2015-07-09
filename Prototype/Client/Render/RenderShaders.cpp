//
//  RenderShaders.cpp
//  Overview
//
//  Created by Samir Sinha on 7/7/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "RenderShaders.hpp"

#include "CKGfx/ShaderLibrary.hpp"

namespace cinek { namespace ovproto {

namespace render {

void registerShaders(gfx::ShaderLibrary& shaderLibrary)
{
    //  should load shaders from a JSON collection to handle uniforms, etc.
    vector<bgfx::UniformHandle> shaderUniforms;
    shaderUniforms.resize(4, BGFX_INVALID_HANDLE);
    shaderUniforms[kShaderUniform_ColorTexture] =
        bgfx::createUniform("u_texColor", bgfx::UniformType::Uniform1iv);
    
    shaderLibrary.loadProgram(
        gfx::makeShaderProgramId(gfx::VertexTypes::kVec3_Normal_Tex0, 0),
        "Shaders/vs_stdpos_uv.bin",
        "Shaders/fs_stdtex_2d.bin",
        std::move(shaderUniforms));

    shaderLibrary.loadProgram(
        gfx::makeShaderProgramId(gfx::VertexTypes::kVec3_Normal_RGBA, 0),
        "Shaders/vs_stdpos_rgba.bin",
        "Shaders/fs_stdcol_rgba.bin",
        std::move(shaderUniforms));

}

}

} /* namespace ovproto */ } /* namespace cinek */