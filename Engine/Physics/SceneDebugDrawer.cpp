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

void SceneDebugDrawer::drawRayTestHit
(
    const SceneRayTestResult& rayTestResult,
    const btVector3& origin,
    btScalar radius,
    bool drawray
)
{
    if (!rayTestResult)
        return;
        
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(rayTestResult.position);
        
    drawSphere(radius, transform, btVector3(0,0.5,1.0f));
    drawLine(rayTestResult.position,
        rayTestResult.position + rayTestResult.normal,
        btVector3(0,0.5,1.0f));
    
    if (drawray) {
        drawLine(origin,
            rayTestResult.position,
            btVector3(0,0,1.0f));
    }
}

void SceneDebugDrawer::flushLines()
{
    CK_ASSERT_RETURN(_camera);
    
    bgfx::setViewRect(_camera->viewIndex,
        _camera->viewportRect.x, _camera->viewportRect.y,
        _camera->viewportRect.w ,_camera->viewportRect.h);
    
    if (_uniforms && _programs) {
        bgfx::TransientVertexBuffer linesTVB;
        bgfx::TransientIndexBuffer linesTIB;
        
        gfx::Matrix4 mainTransform = gfx::Matrix4::kIdentity;
        
         //  draw line buffer
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
        uint16_t* sindices = reinterpret_cast<uint16_t*>(linesTIB.data);
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
            *(sindices++) = vidx++;
            *(sindices++) = vidx++;
        }

        //  simple flat shader for lines
        gfx::Color4 color;
        color.r = _currentLineColor.getX();
        color.g = _currentLineColor.getY();
        color.b = _currentLineColor.getZ();
        color.a = 1.0f;
        
        bgfx::setUniform((*_uniforms)[gfx::kNodeUniformColor], color);

        bgfx::setViewTransform(_camera->viewIndex, _camera->viewMtx, _camera->projMtx);
        bgfx::setTransform(mainTransform);
        bgfx::setVertexBuffer(&linesTVB, 0, (uint32_t)_lineBuffer.size() * 2);
        bgfx::setIndexBuffer(&linesTIB, 0, (uint32_t)_lineBuffer.size() * 2);

        bgfx::setState(
            BGFX_STATE_RGB_WRITE
          | BGFX_STATE_DEPTH_WRITE
          | BGFX_STATE_DEPTH_TEST_LESS
          | BGFX_STATE_PT_LINES
          | BGFX_STATE_MSAA
        );
        
        bgfx::submit(_camera->viewIndex, (*_programs)[gfx::kNodeProgramFlat]);
    }
    
    _lineBuffer.clear();
}



    } /* namespace ove */
} /* namespace cinek */
