//
//  GameStage.cpp
//  Overview
//
//  Created by Samir Sinha on 4/26/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "GameStage.hpp"
#include "RenderResources.hpp"

#include "CKGfx/Mesh.hpp"

namespace cinek { namespace ovproto {

GameStage::GameStage()
{
    _sphere = gfx::createIcoSphere(1.0f, 2, gfx::VertexTypes::kVec3_RGBA);
}

GameStage::~GameStage()
{
}
    
overview::MessageBuffer GameStage::update
(
    overview::MessageBuffer msgBuffer,
    const overview::RenderResources& renderResources
)
{

    gfx::Matrix4 viewMat;
    gfx::Vector3 viewAt = {{ 0.f, 0.f, 0.f }};
    gfx::Vector3 viewEye = {{ 0.0f, 0.0f, -5.f }};

    bx::mtxLookAt(viewMat, viewEye, viewAt);

    gfx::Matrix4 projMat;
    bx::mtxProj(projMat, 60.0f,
        (float)renderResources.viewWidth / renderResources.viewHeight,
        0.1f, 100.f
    );
    bgfx::setViewTransform(0, viewMat, projMat);
    
    //  render mesh
    auto meshElement = _sphere->element(0);
    if (meshElement)
    {
        bgfx::ProgramHandle program = renderResources.shaders.program(0x00000001);
        
        gfx::Matrix4 meshTransform;
        bx::mtxIdentity(meshTransform);
    
        bgfx::setTransform(meshTransform);
        bgfx::setProgram(program);
        bgfx::setVertexBuffer(meshElement->vertBufH);
        bgfx::setIndexBuffer(meshElement->idxBufH);
        bgfx::setState(BGFX_STATE_DEFAULT);
        bgfx::submit(0);
    }
    
    return std::move(msgBuffer);
}

} /* namespace ovproto */ } /* namespace cinek */