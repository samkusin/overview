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
         const bgfx::Memory* indexData);
    ~Mesh();
    
    Mesh(Mesh&& other);
    Mesh& operator=(Mesh&& other);
    
    operator bool() const { return _format != VertexTypes::kInvalid; };

    VertexTypes::Format format() const;
    VertexTypes::Index indexType() const;

private:
    VertexTypes::Format _format;
    VertexTypes::Index _indexType;
    
    bgfx::VertexBufferHandle _vertBufH;
    bgfx::IndexBufferHandle _idxBufH;
};

namespace MeshBuilder {

    struct BuilderState
    {
        //  defines the buffer memory
        const bgfx::VertexDecl* vertexDecl;
        VertexTypes::Index indexType;
        uint32_t vertexLimit;
        uint32_t indexLimit;
        
        //  used by mesh builder
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
        const bgfx::Memory* vertexMemory = nullptr;
        const bgfx::Memory* indexMemory = nullptr;
        
        BuilderState& position(const Vector3& pos);
        BuilderState& normal(const Vector3& normal);
        BuilderState& uv2(const Vector2& uv);
        BuilderState& next();
        template<typename IndexType>
        BuilderState& triangle(IndexType i0, IndexType i1, IndexType i2);
        
        BuilderState& vertex(const float* comp, bgfx::Attrib::Enum attrib);
    };

    BuilderState& create(BuilderState& state);
    
}

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
