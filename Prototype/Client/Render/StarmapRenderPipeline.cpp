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
    
    _starLimit = 1000;
    
    //  http://www.vendian.org/mncharity/dir3/starcolor/details.html
    //
    _colorTable[0 ].fromABGR(0xff9bb2ff);
	_colorTable[1 ].fromABGR(0xff9eb5ff);
	_colorTable[2 ].fromABGR(0xffa3b9ff);
	_colorTable[3 ].fromABGR(0xffaabfff);
	_colorTable[4 ].fromABGR(0xffb2c5ff);
	_colorTable[5 ].fromABGR(0xffbbccff);
	_colorTable[6 ].fromABGR(0xffc4d2ff);
	_colorTable[7 ].fromABGR(0xffccd8ff);
	_colorTable[8 ].fromABGR(0xffd3ddff);
	_colorTable[9 ].fromABGR(0xffdae2ff);
	_colorTable[10].fromABGR(0xffdfe5ff);
	_colorTable[11].fromABGR(0xffe4e9ff);
	_colorTable[12].fromABGR(0xffe9ecff);
	_colorTable[13].fromABGR(0xffeeefff);
	_colorTable[14].fromABGR(0xfff3f2ff);
	_colorTable[15].fromABGR(0xfff8f6ff);
	_colorTable[16].fromABGR(0xfffef9ff);
	_colorTable[17].fromABGR(0xfffff9fb);
	_colorTable[18].fromABGR(0xfffff7f5);
	_colorTable[19].fromABGR(0xfffff5ef);
	_colorTable[20].fromABGR(0xfffff3ea);
	_colorTable[21].fromABGR(0xfffff1e5);
	_colorTable[22].fromABGR(0xffffefe0);
	_colorTable[23].fromABGR(0xffffeddb);
	_colorTable[24].fromABGR(0xffffebd6);
	_colorTable[25].fromABGR(0xffffe9d2);
	_colorTable[26].fromABGR(0xffffe8ce);
	_colorTable[27].fromABGR(0xffffe6ca);
	_colorTable[28].fromABGR(0xffffe5c6);
	_colorTable[29].fromABGR(0xffffe3c3);
	_colorTable[30].fromABGR(0xffffe2bf);
	_colorTable[31].fromABGR(0xffffe0bb);
	_colorTable[32].fromABGR(0xffffdfb8);
	_colorTable[33].fromABGR(0xffffddb4);
	_colorTable[34].fromABGR(0xffffdbb0);
	_colorTable[35].fromABGR(0xffffdaad);
	_colorTable[36].fromABGR(0xffffd8a9);
	_colorTable[37].fromABGR(0xffffd6a5);
	_colorTable[38].fromABGR(0xffffd5a1);
	_colorTable[39].fromABGR(0xffffd29c);
	_colorTable[40].fromABGR(0xffffd096);
	_colorTable[41].fromABGR(0xffffcc8f);
	_colorTable[42].fromABGR(0xffffc885);
	_colorTable[43].fromABGR(0xffffc178);
	_colorTable[44].fromABGR(0xffffb765);
	_colorTable[45].fromABGR(0xffffa94b);
	_colorTable[46].fromABGR(0xffff9523);
	_colorTable[47].fromABGR(0xffff7b00);
	_colorTable[48].fromABGR(0xffff5200);
    
    //  load shader with uniforms
    //  custom uniform definitions here
    _uniforms[kUniform_Scale] = bgfx::createUniform("u_scale", bgfx::UniformType::Uniform1fv);
    _uniforms[kUniform_Texture0] = bgfx::createUniform("u_tex0", bgfx::UniformType::Uniform1iv);
    _uniforms[kUniform_Texture1] = bgfx::createUniform("u_tex1", bgfx::UniformType::Uniform1iv);
    _uniforms[kUniform_ColorLUT] = bgfx::createUniform("u_colortable",
        bgfx::UniformType::Uniform4fv,
        _colorTable.size());
    
    _shaderProgram = resources.shaderLibrary->loadProgram(
        gfx::makeShaderProgramId(kVertexType_Vec3_UV, kShaderIndex_Star),
        "shaders/vs_starmap.vs",
        "shaders/fs_starmap.fs");
    
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
    overview::RenderObjectListReader& objects
)
{
    if (objects.empty())
        return;
    
    if (_shaderProgram)
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
    
    const bgfx::VertexDecl& vertexDecl = gfx::VertexTypes::declaration(kVertexType_Vec3_UV);
    if (!bgfx::allocTransientBuffers(&starTVB, vertexDecl, _starLimit,
                                    &starTIB, _starLimit*6))
    {
        return;
    }
    
    float scale = 1.0f;

    struct Vertex
    {
        float pos[3];       //  x,y,z in world
        float attr[2];      //  0 = abs mag, 1 = color index
    };
    
    struct SystemVisitor : public overview::component::TransformVisitor
    {
        component::Table<overview::component::Renderable> renderables;
        component::Table<component::StarBody> stars;
        Vertex* vertices;
        int vertIndex;
        
        bool visit(overview::Entity e, overview::component::Transform& t)
        {
            auto star = stars.rowForEntity(e);
            if (star)
            {
                auto& vert = vertices[vertIndex++];
                auto renderable = renderables.rowForEntity(e);
                
                //  set position
                vert.pos[0] = renderable->worldSRT.comp[12];
                vert.pos[1] = renderable->worldSRT.comp[13];
                vert.pos[2] = renderable->worldSRT.comp[14];
                
                //  set attributes
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
 
    //  generate star vertices, color table indices and absolute magnitudes
    while (!objects.empty())
    {
        systemVisitor.vertIndex = 0;
        
        for (; !objects.empty() && (systemVisitor.vertIndex - kMaxStarsPerSystem) < _starLimit; )
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
        
        int starCount = systemVisitor.vertIndex;
        if (starCount)
        {
            bgfx::setUniform(_uniforms[kUniform_Scale], &scale);
            bgfx::setUniform(_uniforms[kUniform_ColorLUT], _colorTable.data(), _colorTable.size());
            
            bgfx::TextureHandle texture0 = context.resources->textureAtlas->texture(_starCorona);
            bgfx::setTexture(0, _uniforms[kUniform_Texture0], texture0);
            
            bgfx::ProgramHandle bgfxProgram =
                context.resources->shaderLibrary->program(_shaderProgram);
            bgfx::setProgram(bgfxProgram);
            
            bgfx::setTransform(_mapTransform);
            
            bgfx::setVertexBuffer(&starTVB, 0, starCount);
            bgfx::setIndexBuffer(&starTIB, 0, starCount*6);
            
            bgfx::setState(BGFX_STATE_DEFAULT);
            bgfx::submit(0);
        }
    }
}

} /* namespace ovproto */ } /* namespace cinek */
