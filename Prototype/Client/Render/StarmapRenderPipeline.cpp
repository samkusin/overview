//
//  StarmapRenderPipeline.cpp
//  Overview
//
//  Created by Samir Sinha on 7/9/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "StarmapRenderPipeline.hpp"
#include "Sim/SimDefines.hpp"

#include "Engine/Render/Renderer.hpp"
#include "Engine/Entity/EntityStore.hpp"
#include "Engine/Entity/Comp/Renderable.hpp"
#include "Custom/Comp/StellarSystem.hpp"
#include "Custom/Comp/StarBody.hpp"

#include "Engine/Entity/TransformEntity.hpp"

#include "CKGfx/VertexTypes.hpp"
#include "CKGfx/ShaderLibrary.hpp"
#include "CKGfx/TextureAtlas.hpp"

#include <bgfx/bgfx.h>

namespace cinek { namespace ovproto {

StarmapRenderPipeline::StarmapRenderPipeline() :
    _shaderProgram(kNullHandle),
    _starLimit(0)
{
    _uniforms.fill(BGFX_INVALID_HANDLE);
}
    
void StarmapRenderPipeline::loadResources(overview::RenderResources& resources)
{
    //  if renderer != GL, use a different rendering method than point
    //      sprites (gl_PointSize)
    //  if renderer == DX9 (really? try not to), use PSIZE
    //  if renderer >= DX11, use geo shaders... which requires some extensions
    //      to bgfx
    
    //  determine star limit based on hardware.  for now let's assume a
    //  conservative default (1000?)
    
    _starLimit = 2048;
    
    //  load shader with uniforms
    //  custom uniform definitions here
    _uniforms[kUniform_Scale] = bgfx::createUniform("u_scale", bgfx::UniformType::Vec4);
    _uniforms[kUniform_Texture0] = bgfx::createUniform("u_tex0", bgfx::UniformType::Int1);
    _uniforms[kUniform_Texture1] = bgfx::createUniform("u_tex1", bgfx::UniformType::Int1);
    
    _shaderProgram = resources.shaderLibrary->loadProgram(
        gfx::makeShaderProgramId(gfx::VertexTypes::kVec3_RGBA_Tex0, kShaderIndex_Star),
        "shaders/vs_starmap.bin",
        "shaders/fs_starmap.bin");
    
    _starCorona = resources.textureAtlas->loadTexture("textures/starcorona.png");
    
    bx::mtxIdentity(_mapTransform);
}

void StarmapRenderPipeline::unloadResources(overview::RenderResources& resources)
{
    if (_shaderProgram)
    {
        resources.shaderLibrary->unloadProgram(_shaderProgram);
        _shaderProgram = kNullHandle;
    }
    
    if (_starCorona)
    {
        resources.textureAtlas->unloadTexture(_starCorona);
        _starCorona = nullptr;
    }
}
    
void StarmapRenderPipeline::operator()
(
    overview::RenderContext context,
    const overview::RenderCameraContext& camera,
    overview::RenderObjectListReader& objects
)
{
    if (objects.empty())
        return;

    if (!_shaderProgram)
        return;

    //  a frequently/regularly updated buffer
    //  each vertex is a star point
    //  our strategy is to draw the same vertex six times and generate quad
    //  vertex positions and UVs inside the vertex shader.
    //
    //  until geometry shader support is added, this is likely the most platform
    //  agnostic approach.
    //
    bgfx::TransientVertexBuffer starTVB;
    bgfx::TransientIndexBuffer starTIB;
    
    const bgfx::VertexDecl& vertexDecl = gfx::VertexTypes::declaration(gfx::VertexTypes::kVec3_RGBA_Tex0);
    if (!bgfx::allocTransientBuffers(&starTVB, vertexDecl, _starLimit*4,
                                    &starTIB, _starLimit*6))
    {
        return;
    }
    
    float scale[4] = { 1.0f, 1.0f, 0, 0 };

    struct Vertex
    {
        float x,y,z;        //  x,y,z in world
        uint32_t abgr;      //
        float u,v;          //  0 = abs mag
    };
    
    struct SystemVisitor : public overview::component::TransformVisitor
    {
        component::Table<overview::component::Renderable> renderables;
        component::Table<component::StarBody> stars;
        Vertex* vertices;
        int starIndex;
        uint16_t* indices;
        
        bool visit(overview::Entity e, overview::component::Transform& t)
        {
            auto star = stars.rowForEntity(e);
            if (star)
            {
                int vertIndex = starIndex*4;
                auto renderable = renderables.rowForEntity(e);
                
                auto vert = vertices + vertIndex;
                
                
                //  set position
                vert->x = renderable->worldSRT.comp[12];
                vert->y = renderable->worldSRT.comp[13];
                vert->z = renderable->worldSRT.comp[14];
                
                //  color
                vert->abgr = star->abgrColor;
                
                //  set attributes
                vert->u = star->visualMag;
                
                auto base = vert;
                
                for (vert = vert+3; vert > base; --vert)
                {
                    *vert = *base;
                }
                
                /*
                
                vert[0].x = -2.0f;
                vert[0].y = 2.0f;
                vert[0].z = 5.0f;
                vert[0].abgr = star->abgrColor;
                vert[1].x = 2.0f;
                vert[1].y = 2.0f;
                vert[1].z = 5.0f;
                vert[1].abgr = star->abgrColor;
                vert[2].x = 2.0f;
                vert[2].y = -2.0f;
                vert[2].z = 5.0f;
                vert[2].abgr = star->abgrColor;
                vert[3].x = -2.0f;
                vert[3].y = -2.0f;
                vert[3].z = 5.0f;
                vert[3].abgr = star->abgrColor;
                */
                
                //  6 vertices per star quad - coords calculated in shader.
                auto ind = indices + starIndex*6;
                ind[0] = vertIndex;
                ind[1] = vertIndex+1;
                ind[2] = vertIndex+2;
                ind[3] = vertIndex;
                ind[4] = vertIndex+2;
                ind[5] = vertIndex+3;
                
                ++starIndex;
            }
            return true;
        }
    };
    
    auto renderables = context.entityStore->table<overview::component::Renderable>();
    auto systems = context.entityStore->table<component::StellarSystem>();
    auto transforms = context.entityStore->table<overview::component::Transform>();
    
    SystemVisitor systemVisitor;
    systemVisitor.renderables = renderables;
    systemVisitor.stars = context.entityStore->table<component::StarBody>();
    systemVisitor.vertices = reinterpret_cast<Vertex*>(starTVB.data);
    systemVisitor.indices = reinterpret_cast<uint16_t*>(starTIB.data);
    
    //  generate star vertices, color table indices and absolute magnitudes
    while (!objects.empty())
    {
        systemVisitor.starIndex = 0;
        
        for (; !objects.empty() && (systemVisitor.starIndex - kMaxStarsPerSystem) < _starLimit; )
        {
            auto object = objects.pop();
            auto renderable = renderables.rowAtIndex(object.renderableIdx);
            auto entity = renderable.first;
            
            auto system = systems.rowForEntity(entity);
            if (system)
            {
                systemVisitor(entity, transforms);
            }
        }
        
        
        int starCount = systemVisitor.starIndex;
        if (starCount)
        {
            bgfx::setViewTransform(camera.viewId, camera.mtxView, camera.mtxProj);
        
            bgfx::setUniform(_uniforms[kUniform_Scale], &scale);
            bgfx::TextureHandle texture0 = context.resources->textureAtlas->texture(_starCorona);
            bgfx::setTexture(0, _uniforms[kUniform_Texture0], texture0);
            
            bgfx::ProgramHandle bgfxProgram =
                context.resources->shaderLibrary->program(_shaderProgram);
            bgfx::setProgram(bgfxProgram);

            //  enable alpha blending and disable the depth test since we've
            //  sorted our stars by Z.
            bgfx::setState(
                  BGFX_STATE_RGB_WRITE
                | BGFX_STATE_ALPHA_WRITE
                | BGFX_STATE_CULL_CCW
                | BGFX_STATE_MSAA
                | BGFX_STATE_BLEND_ALPHA
            );

            bgfx::setTransform(_mapTransform);
            
            bgfx::setVertexBuffer(&starTVB, 0, starCount*4);
            bgfx::setIndexBuffer(&starTIB, 0, starCount*6);
            
            bgfx::submit(0);
        }
    }
}

} /* namespace ovproto */ } /* namespace cinek */
