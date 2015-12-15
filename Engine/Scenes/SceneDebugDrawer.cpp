//
//  SceneDebugDrawer.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/6/15.
//
//

#include "SceneDebugDrawer.hpp"
#include "CKGfx/VertexTypes.hpp"

#include "Engine/Debug.hpp"

#include <ckm/math.hpp>

#include <bgfx/bgfx.h>

namespace cinek {
    namespace ove {
    
constexpr int SceneDebugDrawer::kLineBufferSize;
    
SceneDebugDrawer::SceneDebugDrawer() :
    _debugMode(btIDebugDraw::DBG_DrawWireframe |
               btIDebugDraw::DBG_DrawAabb),
    _programs(nullptr),
    _uniforms(nullptr),
    _camera(nullptr)
{
    _lineBuffer.reserve(kLineBufferSize);
    _currentLineColor.setValue(0,0,0);
}
    
SceneDebugDrawer::~SceneDebugDrawer()
{
}
    
void SceneDebugDrawer::drawLine
(
    const btVector3& from1,
    const btVector3& to1,
    const btVector3& color1
)
{
    if (color1 != _currentLineColor || _lineBuffer.size() >= kLineBufferSize) {
        flushLines();
        _currentLineColor = color1;
    }
    
    _lineBuffer.emplace_back(from1, to1);
}

void SceneDebugDrawer::drawContactPoint
(
    const btVector3& PointOnB,
    const btVector3& normalOnB,
    btScalar distance,
    int lifeTime,
    const btVector3& color
)
{
}
     
void SceneDebugDrawer::reportErrorWarning(const char* warningString)
{
    OVENGINE_LOG_WARN(warningString);
}

void SceneDebugDrawer::draw3dText
(
    const btVector3& location,
    const char* textString
)
{
}

void SceneDebugDrawer::setup
(
    const cinek::gfx::NodeRenderer::ProgramMap &programs,
    const cinek::gfx::NodeRenderer::UniformMap &uniforms,
    const cinek::gfx::Camera& camera
)
{
    _programs = &programs;
    _uniforms = &uniforms;
    _camera = &camera;
}

void SceneDebugDrawer::flushLines()
{
    bgfx::TransientVertexBuffer linesTVB;
    bgfx::TransientIndexBuffer linesTIB;
    
    gfx::Matrix4 mainTransform;
    bx::mtxIdentity(mainTransform);
    
     //  draw line buffer
    gfx::Matrix4 viewMtx;
    bx::mtxInverse(viewMtx, _camera->worldMtx);
    
    gfx::Matrix4 projMtx;
    bx::mtxProj(projMtx,
        180.0f * _camera->viewFrustrum.fovRadians()/ckm::pi<float>(),
        _camera->viewFrustrum.aspect(),
        _camera->viewFrustrum.nearZ(),
        _camera->viewFrustrum.farZ(),
        false);

    const bgfx::VertexDecl& vertexDecl =
            gfx::VertexTypes::declaration(gfx::VertexTypes::kVPosition);
    
    if (!bgfx::allocTransientBuffers(&linesTVB,
            vertexDecl,
            kLineBufferSize*2,
            &linesTIB,
            kLineBufferSize*2)) {
            
        OVENGINE_LOG_WARN("SceneDebugDrawer.flushLines() - "
                          "Failed to allocate transient buffers.");
        return;
    }
    
    gfx::Vector3* vertices = reinterpret_cast<gfx::Vector3*>(linesTVB.data);
    uint16_t* indices = reinterpret_cast<uint16_t*>(linesTIB.data);
    uint16_t vidx = 0;
    for (auto it = _lineBuffer.begin(); it != _lineBuffer.end(); ++it) {
        auto& line = *it;
        vertices->x = line.first.getX();
        vertices->y = line.first.getY();
        vertices->z = line.first.getZ();
        ++vertices;
        vertices->x = line.second.getX();
        vertices->y = line.second.getY();
        vertices->z = line.second.getZ();
        ++vertices;
        *(indices++) = vidx++;
        *(indices++) = vidx++;
    }

    //  simple flat shader for lines
    gfx::Color4 color;
    color.r = _currentLineColor.getX();
    color.g = _currentLineColor.getY();
    color.b = _currentLineColor.getZ();
    color.a = 1.0f;
    
    bgfx::setUniform((*_uniforms)[gfx::kNodeUniformColor], color);

    bgfx::setViewTransform(0, viewMtx, projMtx);
    bgfx::setTransform(mainTransform);
    bgfx::setVertexBuffer(&linesTVB, 0, (uint32_t)_lineBuffer.size() * 2);
    bgfx::setIndexBuffer(&linesTIB, 0, (uint32_t)_lineBuffer.size() * 2);

    bgfx::setState(
        BGFX_STATE_RGB_WRITE
      | BGFX_STATE_DEPTH_TEST_LESS
      | BGFX_STATE_PT_LINES
      | BGFX_STATE_MSAA
    );
    
    bgfx::submit(0, (*_programs)[gfx::kNodeProgramFlat]);
    
    _lineBuffer.clear();
}

    } /* namespace ove */
} /* namespace cinek */
