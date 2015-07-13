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
    //  create custom vertex decls
    bgfx::VertexDecl& decl =
        *gfx::VertexTypes::createCustomPreset(kVertexType_Vec3_UV);
    
    decl.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();
    
    //  common shaders
    shaderLibrary.loadProgram(
        gfx::makeShaderProgramId(gfx::VertexTypes::kVec3_Normal_Tex0, 0),
        "shaders/vs_stdpos_uv.bin",
        "shaders/fs_stdtex_2d.bin");

    shaderLibrary.loadProgram(
        gfx::makeShaderProgramId(gfx::VertexTypes::kVec3_Normal_RGBA, 0),
        "shaders/vs_stdpos_rgba.bin",
        "shaders/fs_stdcol_rgba.bin");

}

} /* namespace ovproto */ } /* namespace cinek */