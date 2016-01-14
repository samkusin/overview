//
//  Mesh.hpp
//  Overview
//
//  Created by Samir Sinha on 4/27/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_Mesh_hpp
#define CK_Graphics_Mesh_hpp

#include "GfxTypes.hpp"
#include "VertexTypes.hpp"

#include <cinek/allocator.hpp>

#include <bgfx/bgfx.h>

namespace cinek {
    namespace gfx {

class Mesh
{
    CK_CLASS_NON_COPYABLE(Mesh);

public:
    Mesh();
    Mesh(VertexTypes::Format format,
         VertexTypes::Index indexType,
         const bgfx::Memory* vertexData,
         const bgfx::Memory* indexData,
         PrimitiveType primType);
    ~Mesh();
    
    Mesh(Mesh&& other);
    Mesh& operator=(Mesh&& other);
    
    operator bool() const { return _format != VertexTypes::kInvalid; };

    VertexTypes::Format format() const;
    VertexTypes::Index indexType() const;
    
    bgfx::VertexBufferHandle vertexBuffer() const {
        return _vertBufH;
    }
    bgfx::IndexBufferHandle indexBuffer() const {
        return _idxBufH;
    }
    
    PrimitiveType primitiveType() const {
        return _primitiveType;
    }

private:
    VertexTypes::Format _format;
    VertexTypes::Index _indexType;
    PrimitiveType _primitiveType;
    
    bgfx::VertexBufferHandle _vertBufH;
    bgfx::IndexBufferHandle _idxBufH;
};


/// Generates an IcoSphere mesh using the given format and number of passes.)
///
/// Note for UVs, currently uses sphere mapping to generate U and V coords
/// Future: support multiple types of texture mapping
///
Mesh createIcoSphere
(
    float radius,
    int subdividePasses,
    VertexTypes::Format vertexType
);

Mesh createUVSphere
(
    float radius,
    int32_t numStacks,
    int32_t numSlices,
    VertexTypes::Format vertexType,
    PrimitiveType primitiveType
);

Mesh createQuad
(
    float scale,
    VertexTypes::Format vertexType,
    PrimitiveType primitiveType
);


////////////////////////////////////////////////////////////////////////////////

inline VertexTypes::Format Mesh::format() const {
    return _format;
}

inline VertexTypes::Index Mesh::indexType() const {
    return _indexType;
}
    
    }   // namespace gfx
}   // namespace cinek

#endif /* defined(CK_Graphics_Mesh_hpp) */
