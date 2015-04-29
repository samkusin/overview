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

#include <cinek/vector.hpp>
#include <bgfx/bgfx.h>

namespace cinek {
    namespace gfx {

struct MeshElement
{
    bgfx::VertexBufferHandle vertBufH;
    bgfx::IndexBufferHandle idxBufH;
    Matrix4 transform;
    
    int32_t parentIdx;
    int32_t firstChildIdx;
    int32_t nextSiblingIdx;
};

class Mesh
{
    CK_CLASS_NON_COPYABLE(Mesh);

public:
    Mesh(VertexTypes::Format format,
         const Matrix4& transform,
         const bgfx::Memory* vertexData,
         const bgfx::Memory* indexData,
         uint32_t elementCount = 1,
         const Allocator& allocator=Allocator());
    ~Mesh();
    
    Mesh(Mesh&& other);
    Mesh& operator=(Mesh&& other);

    VertexTypes::Format format() const;

    int32_t addElement(int32_t parentIndex,
                       const Matrix4& transform,
                       const bgfx::Memory* vertexData,
                       const bgfx::Memory* indexData);

    const MeshElement* element(int32_t index) const;

private:
    VertexTypes::Format _format;
    
    struct Node
    {
        MeshElement element;
    };
    vector<Node> _nodes;
    
    int32_t createNode(int parentIndex, const Matrix4& transform,
         const bgfx::Memory* vertexData,
         const bgfx::Memory* indexData);
};

/// Generates an IcoSphere mesh using the given format and number of passes.)
unique_ptr<Mesh> createIcoSphere(float radius, int subdividePasses,
                                 VertexTypes::Format vertexType,
                                 const Vector4& color = {{ 1.f,1.f,1.f,1.f }},
                                 const Allocator& allocator=Allocator());


////////////////////////////////////////////////////////////////////////////////

inline VertexTypes::Format Mesh::format() const
{
    return _format;
}

inline const MeshElement* Mesh::element(int32_t index) const
{
    if (index < 0 || index >= _nodes.size())
        return nullptr;
    
    return &_nodes[index].element;
}

    
    }   // namespace gfx
}   // namespace cinek

#endif /* defined(CK_Graphics_Mesh_hpp) */
