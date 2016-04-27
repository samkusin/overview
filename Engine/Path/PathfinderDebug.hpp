//
//  PathfinderDebug.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/10/16.
//
//

#ifndef Overview_Nav_PathfinderDebug_hpp
#define Overview_Nav_PathfinderDebug_hpp

#include "PathTypes.hpp"

#include "Engine/Contrib/Recast/DebugDraw.h"
#include "CKGfx/NodeRendererTypes.hpp"

namespace cinek {
    namespace ove {
    
struct PathfinderDebug : duDebugDraw
{
    PathfinderDebug(uint32_t primBufSize=128);
    
    virtual ~PathfinderDebug();
	
	virtual void depthMask(bool state);

	virtual void texture(bool state);

	virtual void begin(duDebugDrawPrimitives prim, float size = 1.0f);

	virtual void vertex(const float* pos, unsigned int color);

	virtual void vertex
    (
        const float x, const float y, const float z,
        unsigned int color
    );

	virtual void vertex
    (
        const float* pos,
        unsigned int color,
        const float* uv
    );
	
	virtual void vertex
    (
        const float x, const float y, const float z,
        unsigned int color,
        const float u, const float v
    );
	
	virtual void end();
    
    // should be called prior to calling any rendering methods
    void setup
    (
        const gfx::RenderProgramMap* programs,
        const gfx::RenderUniformMap* uniforms,
        const gfx::Camera* camera,
        gfx::TextureHandle drawTexture
    );
    
private:
    uint32_t _primBufSize;
    uint64_t _drawState;
    bgfx::TransientVertexBuffer _tvb;
    uint32_t _tvbidx;
    uint32_t _tvblimit;
    
    struct VertexInput
    {
        float x;
        float y;
        float z;
        unsigned int abgr;
        float u;
        float v;
    };
    
    //  necessary to support quads (transformed into triangles when filled)
    VertexInput _vcache[4];
    int _vindex;
    bool _textureEnabled;
    
    bool allocateTransientBuffer();
    void flushBuffers();
    
    const cinek::gfx::RenderProgramMap* _programs;
    const cinek::gfx::RenderUniformMap* _uniforms;
    const cinek::gfx::Camera* _camera;
    
    gfx::TextureHandle _drawTexture;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_RecastDebugDraw_hpp */
