//
//  RenderScene.cpp
//  Overview
//
//  Created by Samir Sinha on 5/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "RenderSceneOld.hpp"

#include "Engine/AABB.hpp"
#include "Engine/Entity/EntityStore.hpp"
#include "Engine/Entity/Comp/Transform.hpp"
#include "Engine/Entity/Comp/Renderable.hpp"
#include "Engine/Debug.hpp"

#include "CKGfx/MeshLibrary.hpp"
#include "CKGfx/ShaderLibrary.hpp"
#include "CKGfx/TextureAtlas.hpp"

#include <cinek/vector.hpp>

#include <bgfx/bgfx.h>

namespace cinek { namespace overview {


struct RenderObjectFn
{
    RenderContext& context;
    
    RenderObjectFn(RenderContext& ctx) :
        context(ctx)
    {
    }
    
    void operator()(EntityObject& entity)
    {
        /*
        auto renderable = context.entityDb->componentFromEntity<component::Renderable>(entity.id());
        if (renderable)
        {
            auto& matrix = entity.matrix();
            
            auto mesh = context.resources->meshLibrary->mesh(renderable->meshHandle);
            if (mesh)
            {
                auto meshElement = mesh->element(0);
                bgfx::ProgramHandle program = context.resources->shaderLibrary->program(0x00000001);
                bgfx::UniformHandle uTexColor =
                    context.resources->shaderLibrary->uniformFromProgram(
                        kRenderShaderUniform_ColorTexture,
                        0x00000001
                    );
                gfx::Matrix4 meshTransform;
                bx::mtxMul(meshTransform, meshElement->transform, matrix);

                bgfx::setTransform(meshTransform);
                bgfx::setProgram(program);
                bgfx::setVertexBuffer(meshElement->vertBufH);
                bgfx::setIndexBuffer(meshElement->idxBufH);
                
                bgfx::TextureHandle texture = context.resources->textureAtlas->texture(renderable->texHandle);
                bgfx::setTexture(0, uTexColor, texture);
                bgfx::setState(BGFX_STATE_DEFAULT);
                bgfx::submit(0);
            }
        }
        */
    }
};

void renderScene
(
    RenderContext& context
)
{
    auto& entityStore = *context.entityStore;
   
    //  render scene
    gfx::Matrix4 viewMat;
    gfx::Vector3 viewAt = {{ 0.f, 0.f, 0.f }};
    gfx::Vector3 viewEye = {{ 0.0f, 0.0f, -5.0f }};

    bx::mtxLookAt(viewMat, viewEye, viewAt);

    float aspectXY = (float)context.viewWidth/context.viewHeight;
    gfx::Matrix4 projMat;
    bx::mtxProj(projMat, 60.0f,
        aspectXY,
        0.1f, 100.f
    );
    bgfx::setViewTransform(0, viewMat, projMat);

    /*
    RenderObjectFn renderObjectFn(context);
    entityDb.forEachObject(renderObjectFn);
    */
}

} /* namespace ovproto */ } /* namespace cinek */