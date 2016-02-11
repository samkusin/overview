//
//  PathfinderDebug.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/10/16.
//
//

#include "PathfinderDebug.hpp"
#include "Engine/Debug.hpp"

#include "CKGfx/VertexTypes.hpp"
#include "CKGfx/Camera.hpp"
#include "CKGfx/Texture.hpp"


namespace cinek {
    namespace ove {
    
PathfinderDebug::PathfinderDebug(uint32_t primBufSize) :
    _primBufSize(primBufSize),
    _drawState(0),
    _tvbidx(0),
    _tvblimit(0),
    _vindex(-1),
    _textureEnabled(false),
    _programs(nullptr),
    _uniforms(nullptr),
    _camera(nullptr)
{
}

PathfinderDebug::~PathfinderDebug()
{
}
	
void PathfinderDebug::depthMask(bool state)
{
    if (state) {
        _drawState |= BGFX_STATE_DEPTH_WRITE;
        _drawState |= BGFX_STATE_DEPTH_TEST_LESS;
    }
    else {
        _drawState &= ~BGFX_STATE_DEPTH_WRITE;
        _drawState &= ~BGFX_STATE_DEPTH_TEST_MASK;
    }
}

void PathfinderDebug::texture(bool state)
{
    _textureEnabled = state;
}


void PathfinderDebug::begin(duDebugDrawPrimitives prim, float size)
{
    CK_ASSERT(_vindex < 0);
    
    switch (prim) {
    
    case DU_DRAW_POINTS:
        _drawState |= BGFX_STATE_PT_POINTS;
        _drawState |= BGFX_STATE_POINT_SIZE((unsigned int)size);
        break;
    case DU_DRAW_LINES:
        //  note bgfx doesn't seem to support glLineWidth internally for GL
        //  not sure about other impls.  but we'll set the point size anyway
        //  just in case.
        _drawState |= BGFX_STATE_PT_LINES;
        _drawState |= BGFX_STATE_POINT_SIZE((unsigned int)size);
        break;
    case DU_DRAW_TRIS:
        break;
    case DU_DRAW_QUADS:
        _vindex = 0;    // use the vertex cache to generate complex primitives
        break;
    default:
        CK_ASSERT(false);
        break;
        
    }
    
    //printf("Begin\n");
    
    allocateTransientBuffer();
}

bool PathfinderDebug::allocateTransientBuffer()
{
    const bgfx::VertexDecl& vertexDecl = _textureEnabled ?
        gfx::VertexTypes::declaration(gfx::VertexTypes::kVTex0) :
        gfx::VertexTypes::declaration(gfx::VertexTypes::kVPositionColor);

    _tvbidx = 0;
    _tvblimit = 0;
    
    if ((_drawState & BGFX_STATE_PT_POINTS) == BGFX_STATE_PT_POINTS) {
        _tvblimit = _primBufSize;
    }
    else if ((_drawState & BGFX_STATE_PT_LINES) == BGFX_STATE_PT_LINES) {
        _tvblimit = _primBufSize * 2;
    }
    else {
        _tvblimit = _primBufSize * ((_vindex < 0) ? 3 : 6);
    }
    
    memset(&_tvb, 0, sizeof(_tvb));
    
    if (!checkAvailTransientVertexBuffer(_tvblimit, vertexDecl)) {
        OVENGINE_LOG_WARN("RecastDebugDraw - transient buffer of size %u not available!",
                      _tvblimit);
        _drawState &= ~BGFX_STATE_PT_MASK;
        return false;
    }
    allocTransientVertexBuffer(&_tvb, _tvblimit, vertexDecl);
    return true;
}

void PathfinderDebug::vertex(const float* pos, unsigned int color)
{
    vertex(pos[0], pos[1], pos[2], color);
}

void PathfinderDebug::vertex
(
    const float x, const float y, const float z,
    unsigned int color
)
{
    CK_ASSERT(!_textureEnabled);
    if (!_tvb.data)
        return;
    
    uint32_t needed = _vindex < 0 ? 1 : _vindex == 3 ? 6 : 0;
    if (_tvbidx + needed > _tvblimit) {
        flushBuffers();
        if (!allocateTransientBuffer())
            return;
    }
    
    if (_vindex < 0) {
        auto vtx = reinterpret_cast<gfx::VertexTypes::PositionColor*>(_tvb.data) + _tvbidx;
        vtx->x = x;
        vtx->y = y;
        vtx->z = z;
        vtx->abgr = color;
        
        //printf("\t(%.2f,%.2f,%2f)\n", x, y, z);
    }
    else {
        CK_ASSERT(_vindex < sizeof(_vcache)/sizeof(_vcache[0]));
        VertexInput& inVertex = _vcache[_vindex++];
        inVertex.x = x;
        inVertex.y = y;
        inVertex.z = z;
        inVertex.abgr = color;
        
        if (_vindex >= 4) {
            //  make two tris given a clockwise (recast winding order) quad
            auto vtx = reinterpret_cast<gfx::VertexTypes::PositionColor*>(_tvb.data) + _tvbidx;
            const VertexInput& v0 = _vcache[0];
            const VertexInput& v1 = _vcache[1];
            const VertexInput& v2 = _vcache[2];
            const VertexInput& v3 = _vcache[3];
            
            vtx[0].x = v0.x;
            vtx[0].y = v0.y;
            vtx[0].z = v0.z;
            vtx[0].abgr = v0.abgr;
            vtx[1].x = v1.x;
            vtx[1].y = v1.y;
            vtx[1].z = v1.z;
            vtx[1].abgr = v1.abgr;
            vtx[2].x = v2.x;
            vtx[2].y = v2.y;
            vtx[2].z = v2.z;
            vtx[2].abgr = v2.abgr;
            vtx[3] = vtx[0];
            vtx[4] = vtx[2];
            vtx[5].x = v3.x;
            vtx[5].y = v3.y;
            vtx[5].z = v3.z;
            vtx[5].abgr = v3.abgr;
            
            _vindex = 0;
        }
    }
    
    _tvbidx += needed;
}

void PathfinderDebug::vertex
(
    const float* pos,
    unsigned int color,
    const float* uv
)
{
    vertex(pos[0], pos[1], pos[2], color, uv[0], uv[1]);
}

void PathfinderDebug::vertex
(
    const float x, const float y, const float z,
    unsigned int color,
    const float u, const float v
)
{
    CK_ASSERT(_textureEnabled);
    
    if (!_tvb.data)
        return;
    
    uint32_t needed = _vindex < 0 ? 1 : _vindex == 3 ? 6 : 0;
    if (_tvbidx + needed > _tvblimit) {
        flushBuffers();
        if (!allocateTransientBuffer())
            return;
    }
    
    if (_vindex < 0) {
        auto vtx = reinterpret_cast<gfx::VertexTypes::PositionColorUV*>(_tvb.data) + _tvbidx;
        vtx->x = x;
        vtx->y = y;
        vtx->z = z;
        vtx->abgr = color;
        vtx->u = u;
        vtx->v = v;
    }
    else {
        CK_ASSERT(_vindex < sizeof(_vcache)/sizeof(_vcache[0]));
        VertexInput& inVertex = _vcache[_vindex++];
        inVertex.x = x;
        inVertex.y = y;
        inVertex.z = z;
        inVertex.abgr = color;
        inVertex.u = u;
        inVertex.v = v;
        
        if (_vindex >= 4) {
            //  make two tris given a clockwise (recast winding order) quad
            auto vtx = reinterpret_cast<gfx::VertexTypes::PositionColorUV*>(_tvb.data) + _tvbidx;
            const VertexInput& v0 = _vcache[0];
            const VertexInput& v1 = _vcache[1];
            const VertexInput& v2 = _vcache[2];
            const VertexInput& v3 = _vcache[3];
            
            vtx[0].x = v0.x;
            vtx[0].y = v0.y;
            vtx[0].z = v0.z;
            vtx[0].abgr = v0.abgr;
            vtx[0].u = v0.u;
            vtx[0].v = v0.v;
            vtx[1].x = v1.x;
            vtx[1].y = v1.y;
            vtx[1].z = v1.z;
            vtx[1].abgr = v1.abgr;
            vtx[1].u = v1.u;
            vtx[1].v = v1.v;
            vtx[2].x = v2.x;
            vtx[2].y = v2.y;
            vtx[2].z = v2.z;
            vtx[2].abgr = v2.abgr;
            vtx[2].u = v2.u;
            vtx[2].v = v2.v;
            vtx[3] = vtx[0];
            vtx[4] = vtx[2];
            vtx[5].x = v3.x;
            vtx[5].y = v3.y;
            vtx[5].z = v3.z;
            vtx[5].abgr = v3.abgr;
            vtx[5].u = v3.u;
            vtx[5].v = v3.v;
            
            _vindex = 0;
        }
    }
    
    _tvbidx += needed;
}

void PathfinderDebug::end()
{
    flushBuffers();
    
    _drawState &= ~BGFX_STATE_PT_MASK;
    _drawState &= ~BGFX_STATE_POINT_SIZE_MASK;
    _vindex = -1;
    
    //printf("End\n");
}

void PathfinderDebug::setup
(
    const gfx::RenderProgramMap *programs,
    const gfx::RenderUniformMap *uniforms,
    const gfx::Camera *camera,
    gfx::TextureHandle drawTexture
)
{
    _programs = programs;
    _uniforms = uniforms;
    _camera = camera;
    _drawTexture = drawTexture;
}


void PathfinderDebug::flushBuffers()
{
    CK_ASSERT_RETURN(_camera != nullptr);
    if (!_tvb.data)
        return;
    bgfx::setViewRect(_camera->viewIndex,
        _camera->viewportRect.x, _camera->viewportRect.y,
        _camera->viewportRect.w ,_camera->viewportRect.h);
 
    gfx::Matrix4 mainTransform = gfx::Matrix4::kIdentity;
    
    if (_drawTexture && _textureEnabled) {
        bgfx::setTexture(0, (*_uniforms)[gfx::kNodeUniformTexDiffuse], _drawTexture->bgfxHandle());
    }
    
    bgfx::setViewTransform(_camera->viewIndex, _camera->viewMtx, _camera->projMtx);
    bgfx::setTransform(mainTransform);
    bgfx::setVertexBuffer(&_tvb, 0, _tvbidx);
    
    bgfx::setState(
        _drawState
      | BGFX_STATE_RGB_WRITE
      | BGFX_STATE_MSAA
      | BGFX_STATE_CULL_CCW
    );
    
    if (_textureEnabled) {
        bgfx::submit(_camera->viewIndex, (*_programs)[gfx::kNodeProgramDiffuse]);
    }
    else {
        bgfx::submit(_camera->viewIndex, (*_programs)[gfx::kNodeProgramColor]);
    }
}

    
    } /* namespace ove */
} /* namespace cinek */