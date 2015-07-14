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

void registerShaders(gfx::ShaderLibrary& shaderLibrary)
{
    //  common shaders
    shaderLibrary.loadProgram(
        gfx::makeShaderProgramId(gfx::VertexTypes::kVec3_Normal_Tex0, 0),
        "shaders/vs_std_uv.bin",
        "shaders/fs_std_tex.bin");

    shaderLibrary.loadProgram(
        gfx::makeShaderProgramId(gfx::VertexTypes::kVec3_Normal_RGBA, 0),
        "shaders/vs_std_rgba.bin",
        "shaders/fs_std_rgba.bin");
}

} /* namespace ovproto */ } /* namespace cinek */