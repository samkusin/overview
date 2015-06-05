//
//  GameStage.cpp
//  Overview
//
//  Created by Samir Sinha on 4/26/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "GameStage.hpp"
#include "RenderResources.hpp"


#include <bgfx/bgfx.h>

namespace cinek { namespace ovproto {

//  use a larger output buffer in prep for more events than inputs
//
GameStage::GameStage()
{
    //_sphere = gfx::createIcoSphere(1.0f, 3, gfx::VertexTypes::kVec3_Normal);
}

GameStage::~GameStage()
{
}
    
void GameStage::update(overview::EngineContext& context)
{
    //  render local scene
    //render(renderResources);
}

/*
void GameStage::render(overview::RenderResources& resources)
{
    //  render scene
    gfx::Matrix4 viewMat;
    gfx::Vector3 viewAt = {{ 0.f, 0.f, 0.f }};
    gfx::Vector3 viewEye = {{ 0.0f, 0.0f, -5.0f }};

    bx::mtxLookAt(viewMat, viewEye, viewAt);

    gfx::Matrix4 projMat;
    bx::mtxProj(projMat, 60.0f,
        (float)resources.viewWidth / resources.viewHeight,
        0.1f, 100.f
    );
    bgfx::setViewTransform(0, viewMat, projMat);
    
    //  render mesh
    auto meshElement = _sphere->element(0);
    if (meshElement)
    {
        bgfx::ProgramHandle program = resources.shaders.program(0x00000001);
        
        gfx::Matrix4 meshTransform = meshElement->transform;
        bgfx::setTransform(meshTransform);
        bgfx::setProgram(program);
        bgfx::setVertexBuffer(meshElement->vertBufH);
        bgfx::setIndexBuffer(meshElement->idxBufH);
        bgfx::setState(BGFX_STATE_DEFAULT);
        bgfx::submit(0);
    }
}
*/

} /* namespace ovproto */ } /* namespace cinek */