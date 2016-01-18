//
//  Geometry.cpp
//  SampleCommon
//
//  Created by Samir Sinha on 1/12/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

#include "Geometry.hpp"

#include <ckm/math.hpp>

#include <bgfx/bgfx.h>
#include <bx/fpumath.h>

/*
 *  - Rendering culls clockwise (CW) faces by default.  So winding order should
 *    be counter-clockwise (CCW)
 *  - Rendering assumes a LH coordinate system with +Z as forward
 *  - UV coordinates are lower-left to upper-right, for example (0,0) to (1,1)
 */

namespace cinek {
    namespace gfx {
    
namespace MeshBuilder
{

    BuilderState& vertex
    (
        BuilderState& state,
        const float* comp,
        bgfx::Attrib::Enum attrib
    )
    {
        CK_ASSERT_RETURN_VALUE(state.vertexCount < state.vertexLimit, state);
        bool valid = state.vertexDecl->has(attrib);
        if (valid) {
            bgfx::vertexPack(comp, false, attrib,
                             *state.vertexDecl,
                             state.type == BuilderState::kMemory ? state.vertexMemory->data
                                                   : state.vertexBuffer,
                             state.vertexCount);
        }
        return state;
    }

    BuilderState& BuilderState::position(const Vector3& pos)
    {
        return vertex(*this, pos.comp, bgfx::Attrib::Position);
    }
    
    BuilderState& BuilderState::normal(const Vector3& normal)
    {
        return vertex(*this, normal.comp, bgfx::Attrib::Normal);
    }
    
    BuilderState& BuilderState::uv2(const Vector2& uv)
    {
        return vertex(*this, uv.comp, bgfx::Attrib::TexCoord0);
    }
    
    BuilderState& BuilderState::addweight(uint16_t boneIndex, float weight)
    {
        CK_ASSERT_RETURN_VALUE(weightStackIdx < 4, *this);
        
        indices.comp[weightStackIdx] = (float)(boneIndex);
        weights.comp[weightStackIdx] = weight;
        ++weightStackIdx;
        
        return *this;
    }
    
    BuilderState& BuilderState::endweights()
    {
        while (weightStackIdx < 4) {
            indices.comp[weightStackIdx] = 0.0f;
            weights.comp[weightStackIdx] = 0.0f;
            ++weightStackIdx;
        }
        vertex(*this, indices.comp, bgfx::Attrib::Indices);
        vertex(*this, weights.comp, bgfx::Attrib::Weight);
        
        weightStackIdx = 0;
        return *this;
    }
    
    BuilderState& BuilderState::next()
    {
        ++vertexCount;
        return *this;
    }
    
    template<>
    BuilderState& BuilderState::triangle<uint16_t>
    (
        uint16_t i0, uint16_t i1, uint16_t i2
    )
    {
        CK_ASSERT_RETURN_VALUE(indexCount + 3 <= indexLimit, *this);
        CK_ASSERT(indexType == VertexTypes::kIndex16);
        uint16_t* ptr = reinterpret_cast<uint16_t*>(type == kMemory ? indexMemory->data
                                                                    : indexBuffer)
                                                    + indexCount;
        ptr[0] = i0;
        ptr[1] = i1;
        ptr[2] = i2;
        indexCount += 3;
        return *this;
    }
    
    template<>
    BuilderState& BuilderState::triangle<uint32_t>
    (
        uint32_t i0, uint32_t i1, uint32_t i2
    )
    {
        CK_ASSERT_RETURN_VALUE(indexCount + 3 <= indexLimit, *this);
        CK_ASSERT(indexType == VertexTypes::kIndex32);
        uint32_t* ptr = reinterpret_cast<uint32_t*>(type == kMemory ? indexMemory->data
                                                                    : indexBuffer)
                                                    + indexCount;
        ptr[0] = i0;
        ptr[1] = i1;
        ptr[2] = i2;
        indexCount += 3;
        return *this;
    }
    
    template<>
    BuilderState& BuilderState::line<uint16_t>
    (
        uint16_t i0, uint16_t i1
    )
    {
        CK_ASSERT_RETURN_VALUE(indexCount + 2 <= indexLimit, *this);
        CK_ASSERT(indexType == VertexTypes::kIndex16);
        uint16_t* ptr = reinterpret_cast<uint16_t*>(type == kMemory ? indexMemory->data
                                                                    : indexBuffer)
                                                    + indexCount;
        ptr[0] = i0;
        ptr[1] = i1;
        indexCount += 2;
        return *this;
    }
    
    template<>
    BuilderState& BuilderState::line<uint32_t>
    (
        uint32_t i0, uint32_t i1
    )
    {
        CK_ASSERT_RETURN_VALUE(indexCount + 2 <= indexLimit, *this);
        CK_ASSERT(indexType == VertexTypes::kIndex32);
        uint32_t* ptr = reinterpret_cast<uint32_t*>(type == kMemory ? indexMemory->data
                                                                    : indexBuffer)
                                                    + indexCount;
        ptr[0] = i0;
        ptr[1] = i1;
        indexCount += 2;
        return *this;
    }

    BuilderState& create(BuilderState& state, Counts counts)
    {
        CK_ASSERT_RETURN_VALUE(state.indexType != VertexTypes::kIndex0, state);
        
        state.vertexLimit = counts.vertexCount;
        state.indexLimit = counts.indexCount;;
        
        uint32_t sz = state.vertexDecl->getSize(state.vertexLimit);
        state.vertexMemory = bgfx::alloc(sz);
        state.vertexCount = 0;
        state.weightStackIdx = 0;
        
        if (state.indexType == VertexTypes::kIndex16) {
            sz = sizeof(uint16_t)*state.indexLimit;
        }
        else if (state.indexType == VertexTypes::kIndex32) {
            sz = sizeof(uint32_t)*state.indexLimit;
        }
        else {
            CK_ASSERT(false);
            sz = sizeof(uint16_t)*state.indexLimit;
        }
        
        state.indexMemory = bgfx::alloc(sz);
        state.indexCount = 0;
        
        state.type = BuilderState::kMemory;
        
        return state;
    }
    
    BuilderState& createWithBuffers
    (
        BuilderState& state,
        uint8_t* vertexBuffer,
        int32_t vertexStart,
        int32_t vertexLimit,
        uint8_t* indexBuffer,
        int32_t indexStart,
        int32_t indexLimit
    )
    {
        CK_ASSERT_RETURN_VALUE(state.indexType != VertexTypes::kIndex0, state);
        
        state.vertexLimit = vertexLimit;

        state.vertexBuffer = vertexBuffer;
        state.vertexCount = vertexStart;
        
        state.indexBuffer = indexBuffer;
        state.indexLimit = indexLimit;
        state.indexCount = indexStart;
        
        state.weightStackIdx = 0;
        
        state.type = BuilderState::kBuffer;
        
        return state;
    }
    
    /*
     *  A good amount of the geometry generation code uses techniques from
     *  FreeGLUT for expediency.  We try to minimize or eliminate any allocation
     *  of memory for our building functions.  But the basic concepts like UV
     *  sphere generation were adapted from the FreeGLUT project.
     *
     *  http://sourceforge.net/projects/freeglut/
     *      - MIT License
     */
    Counts calculateUVSphereCounts
    (
        int32_t stackCnt,
        int32_t sliceCnt,
        PrimitiveType primType
    )
    {
        Counts counts;
        
        CK_ASSERT(sliceCnt>0 && stackCnt >2);
        
        if (primType == PrimitiveType::kTriangles) {
            //  tris only
            counts.vertexCount = stackCnt * sliceCnt;
            counts.indexCount = (stackCnt-3) * 6 * (sliceCnt-1); // excludes top and bottom
            counts.indexCount += 6 * (sliceCnt-1);               // includes top and bottom
        }
        else if (primType == PrimitiveType::kLines) {
            //  tris and quads as lines
            counts.vertexCount = stackCnt * sliceCnt;
            counts.indexCount = (stackCnt-3) * 4 * (sliceCnt-1); // excludes top and bottom
            counts.indexCount += 6 * (sliceCnt-1);               // includes top and bottom
            counts.indexCount *= 2;                              // each line requires 2 indices
        }
        else {
            CK_ASSERT(false);
        }
        
        return counts;
    }
    
    template<typename IndexType>
    void buildUVSphereIndices
    (
        BuilderState& state,
        IndexType baseIndex,
        int32_t stackCnt,
        int32_t sliceCnt,
        PrimitiveType primType
    )
    {
        //  split into primitives
        if (primType == PrimitiveType::kTriangles) {
            //  bottom first
            int32_t stack = 0;
            for (int32_t slice = 0; slice < sliceCnt-1; ++slice) {
                IndexType i0 = baseIndex + (IndexType)(stack * sliceCnt + slice);
                state.triangle<IndexType>(i0, i0+sliceCnt+1, i0+sliceCnt);
//              printf("(%u,%u,%u)\n", i0, i0+sliceCnt+1, i0+sliceCnt);
            }
            
            ++stack;
            
            for (; stack < stackCnt-2; ++stack) {
                for (int32_t slice = 0; slice < sliceCnt-1; ++slice) {
                    IndexType i0 = baseIndex + (IndexType)(stack * sliceCnt + slice);
                    state.triangle<IndexType>(i0, i0+1, i0+sliceCnt+1);
                    state.triangle<IndexType>(i0, i0+sliceCnt+1, i0+sliceCnt);
//                  printf("(%u,%u,%u)\n", i0, i0+1, i0+sliceCnt+1);
//                  printf("(%u,%u,%u)\n", i0, i0+sliceCnt+1, i0+sliceCnt);
                    
                }
            }
            //  top last
            for (int32_t slice = 0; slice < sliceCnt-1; ++slice) {
                IndexType i0 = baseIndex + (IndexType)(stack * sliceCnt + slice);
                state.triangle<IndexType>(i0, i0+1, i0+sliceCnt);
//              printf("(%u,%u,%u)\n", i0, i0+1, i0+sliceCnt);
            }
        }
        else if (primType == PrimitiveType::kLines) {
            //  hardly optimal - overlapping lines instead of a true wireframe
            //  since we use the same algorithm to generate triangle-based
            //  mesh spheres.
            //
            //  TODO - optimize to limit the number of vertices used
            //
            int32_t stack = 0;
            for (int32_t slice = 0; slice < sliceCnt-1; ++slice) {
                IndexType i0 = baseIndex + (IndexType)(stack * sliceCnt + slice);
                state.line<IndexType>(i0, i0+sliceCnt+1);
                state.line<IndexType>(i0+sliceCnt+1, i0+sliceCnt);
                state.line<IndexType>(i0+sliceCnt, i0);
            }
            
            ++stack;
            
            for (; stack < stackCnt-2; ++stack) {
                for (int32_t slice = 0; slice < sliceCnt-1; ++slice) {
                    IndexType i0 = baseIndex + (IndexType)(stack * sliceCnt + slice);
                    state.line<IndexType>(i0, i0+1);
                    state.line<IndexType>(i0+1, i0+sliceCnt+1);
                    state.line<IndexType>(i0+sliceCnt+1, i0+sliceCnt);
                    state.line<IndexType>(i0+sliceCnt, i0);
                }
            }
            //  top last
            for (int32_t slice = 0; slice < sliceCnt-1; ++slice) {
                IndexType i0 = baseIndex + (IndexType)(stack * sliceCnt + slice);
                state.line<IndexType>(i0, i0+1);
                state.line<IndexType>(i0+1, i0+sliceCnt);
                state.line<IndexType>(i0+sliceCnt, i0);
//              printf("(%u,%u,%u)\n", i0, i0+1, i0+sliceCnt);
            }
        }
        else {
            CK_ASSERT(false);
        }

    }
    
    BuilderState& buildUVSphere
    (
        BuilderState& state,
        float radius,
        int32_t stackCnt,
        int32_t sliceCnt,
        PrimitiveType primType
    )
    {
        CK_ASSERT_RETURN_VALUE(sliceCnt>0 && stackCnt >2, state);
        
        Counts space = calculateUVSphereCounts(stackCnt, sliceCnt, primType);
        CK_ASSERT_RETURN_VALUE(state.hasSpaceFor(space), state);
      
        uint32_t baseIndex = state.vertexCount;
        
        //  stacks
        const float kStackFraction = 1.0f/(stackCnt-1);
        const float kSliceFraction = 1.0f/(sliceCnt-1);
        
        for (int32_t stack = 0; stack < stackCnt; ++stack) {
            const float kStackAngle = ckm::pi<float>() * stack * kStackFraction;
            const float kSinStack = ckm::sin(kStackAngle);
            
            for (int32_t slice = 0; slice < sliceCnt; ++slice) {
                const float kSliceAngle = ckm::pi<float>() * slice * kSliceFraction;
                Vector3 norm;
                norm.x = ckm::cos(2*kSliceAngle) * kSinStack;
                norm.z = ckm::sin(2*kSliceAngle) * kSinStack;
                norm.y = ckm::sin(-ckm::pi<float>()*0.5f + kStackAngle);
            
                Vector3 pos = norm * radius;
                state.position(pos);
                state.normal(norm);
     
                //printf("(%d,%d): (%.2f, %.2f, %.2f)\n", stack, slice, pos.x,pos.y,pos.z);
     
                if (state.vertexDecl->has(bgfx::Attrib::TexCoord0)) {
                    state.uv2({ slice * kSliceFraction, stack * kStackFraction });
                }
                
                state.next();
            }
        }
        
        //  split into primitives
        if (state.indexType == VertexTypes::kIndex16) {
            buildUVSphereIndices<uint16_t>(state, (uint16_t)baseIndex,
                stackCnt, sliceCnt, primType);
        }
        else {
            buildUVSphereIndices<uint32_t>(state, baseIndex,
                stackCnt, sliceCnt, primType);
        }
        return state;
    }
    
}

    } /* namespace gfx */
} /* namespace cinek */
