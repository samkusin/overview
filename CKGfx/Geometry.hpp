//
//  Geometry.hpp
//  SampleCommon
//
//  Created by Samir Sinha on 1/12/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_Geometry_hpp
#define CK_Graphics_Geometry_hpp

#include "GfxTypes.hpp"
#include "VertexTypes.hpp"

namespace bgfx {
    struct VertexDecl;
    struct Memory;
}

namespace cinek {
    namespace gfx {
    
namespace MeshBuilder {
    
    struct Counts
    {
        int32_t vertexCount;
        int32_t indexCount;
    };

    struct BuilderState
    {
        CK_CLASS_NON_COPYABLE(BuilderState);
        
        BuilderState() = default;
        
        //  defines the buffer vertex and index types
        //  must be specified prior to creation
        const bgfx::VertexDecl* vertexDecl;
        VertexTypes::Index indexType;
        
        //  used by mesh builder
        int32_t vertexLimit;
        int32_t indexLimit;
        int32_t vertexCount;
        int32_t indexCount;
        
        enum
        {
            kMemory,
            kBuffer
        }
        type;
       
        union
        {
            const bgfx::Memory* vertexMemory;
            uint8_t* vertexBuffer;
        };
        union
        {
            const bgfx::Memory* indexMemory;
            uint8_t* indexBuffer;
        };
        
        Vector4 indices;
        Vector4 weights;
        uint16_t weightStackIdx;
        
        BuilderState& position(const Vector3& pos);
        BuilderState& normal(const Vector3& normal);
        BuilderState& uv2(const Vector2& uv);
        BuilderState& addweight(uint16_t boneIndex, float weight);
        BuilderState& endweights();
        BuilderState& next();
        template<typename IndexType>
        BuilderState& triangle(IndexType i0, IndexType i1, IndexType i2);
        template<typename IndexType>
        BuilderState& line(IndexType i0, IndexType i1);
        
        Counts spaceRemaining() const {
            return { vertexLimit - vertexCount, indexLimit - indexCount };
        }
        
        bool hasSpaceFor(Counts counts) const {
            Counts remaining = spaceRemaining();
            return remaining.vertexCount >= counts.vertexCount &&
                   remaining.indexCount >= counts.indexCount;
        }
    };


    BuilderState& create
    (
        BuilderState& state,
        Counts counts
    );

    BuilderState& createWithBuffers
    (
        BuilderState& state,
        uint8_t* vertexBuffer,
        int32_t vertexStart,
        int32_t vertexLimit,
        uint8_t* indexBuffer,
        int32_t indexStart,
        int32_t indexLimit
    );
    
    Counts calculateUVSphereCounts
    (
        int32_t numStacks,
        int32_t numSlices,
        PrimitiveType primType
    );
 
    BuilderState& UVSphere
    (
        BuilderState& state,
        float radius,
        int32_t numStacks,
        int32_t numSlices,
        PrimitiveType primType
    );
    
}
    
    } /* namespace gfx */
} /* namespace cinek */

#endif /* Geometry_hpp */
