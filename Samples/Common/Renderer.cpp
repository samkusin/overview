//
//  Renderer.cpp
//  SampleCommon
//
//  Created by Samir Sinha on 11/8/15.
//  Copyright © 2015 Cinekine. All rights reserved.
//

#include "Renderer.hpp"

#include "CKGfx/ShaderLibrary.hpp"
#include "CKGfx/ModelJsonSerializer.hpp"
#include "CKGfx/Shaders/ckgfx.sh"

#include <cinek/filestreambuf.hpp>
#include <ckjson/jsonstreambuf.hpp>
#include <ckjson/json.hpp>

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


void registerShaders
(
    cinek::gfx::ShaderLibrary& shaderLibrary,
    cinek::gfx::NodeRenderer::ProgramMap& programs,
    cinek::gfx::NodeRenderer::UniformMap& uniforms,
    const std::vector<SampleShaderProgram>& configs
)
{
    programs.fill(BGFX_INVALID_HANDLE);
    
    
    for (auto& config : configs) {
        shaderLibrary.loadProgram(config.programId,
            config.vsBinaryPath,
            config.fsBinaryPath);
        
        programs[config.programSlot] = shaderLibrary.program(config.programId);
    }
    
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
    uniforms[cinek::gfx::kNodeUniformColor] =
            bgfx::createUniform("u_color", bgfx::UniformType::Vec4, 1);
}
