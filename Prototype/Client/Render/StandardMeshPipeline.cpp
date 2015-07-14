//
//  StandardMeshPipeline.cpp
//  Overview
//
//  Created by Samir Sinha on 7/13/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "StandardMeshPipeline.hpp"

#include "Engine/Render/Renderer.hpp"
#include "Engine/Entity/EntityStore.hpp"
#include "Engine/Entity/Comp/Renderable.hpp"
#include "Engine/Entity/Comp/MeshRenderable.hpp"

#include "Engine/Entity/TransformEntity.hpp"

#include "CKGfx/VertexTypes.hpp"
#include "CKGfx/ShaderLibrary.hpp"
#include "CKGfx/TextureAtlas.hpp"

#include <bgfx/bgfx.h>

namespace cinek { namespace ovproto {

StandardMeshPipeline::StandardMeshPipeline()
{
}
    
void StandardMeshPipeline::loadResources(overview::RenderResources& resources)
{
 
    //  uses the standard programs
 //   resources.shaderLibrary->
   
    /*
    bgfx::getShaderUniforms(<#bgfx::ShaderHandle _handle#>)
    bgfx::createUniform(<#const char *_name#>, <#UniformType::Enum _type#>)
    //  create uniforms
    _uniforms[kUniform_Scale] = bgfx::createUniform("u_scale", bgfx::UniformType::Vec4);
    _uniforms[kUniform_Texture0] = bgfx::createUniform("u_tex0", bgfx::UniformType::Int1);
    _uniforms[kUniform_Texture1] = bgfx::createUniform("u_tex1", bgfx::UniformType::Int1);
    */
}

void StandardMeshPipeline::unloadResources(overview::RenderResources& resources)
{
}
    
void StandardMeshPipeline::operator()
(
    overview::RenderContext context,
    const overview::RenderCameraContext& camera,
    overview::RenderObjectListReader& objects
)
{
}

} /* namespace ovproto */ } /* namespace cinek */