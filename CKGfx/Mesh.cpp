//
//  Mesh.cpp
//  Overview
//
//  Created by Samir Sinha on 4/27/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "Mesh.hpp"

#include <cinek/debug.hpp>
#include <cinek/map.hpp>
#include <cinek/vector.hpp>
#include <bx/fpumath.h>

namespace cinek {
    namespace gfx {

template<typename KeyType, typename IndexType, int BitShift>
struct IcoSphereUtility
{
    using index_type = IndexType;
    using key_type = KeyType;
    
    unordered_map<KeyType, IndexType> midpointIndices;
    vector<Vector3> vertices;
    vector<IndexType> indices;
    
    IcoSphereUtility(int passes, const Allocator& allocator) :
        midpointIndices(allocator),
        vertices(allocator),
        indices(allocator)
    {
        vertices.reserve(12 + 30 * passes*passes);
        //  We're using the same index vector for current indices
        //  and the new index vector created after a recursive/subdivide pass.
        //  So make room for additional indices by adding an extra pass.
        int faceMult = 1;
        for (int i = 1; i <= passes+1; ++i)
            faceMult *= 4;
        
        indices.reserve( 3 * (20 * faceMult));
    }
    
    IndexType addVertex(const Vector3& v)
    {
        Vector3 vn;
        bx::vec3Norm((float*)vn, (const float*)v);
        vertices.emplace_back(vn);
        return static_cast<IndexType>(vertices.size()-1);
    }
    
    IndexType addMiddlePoint(IndexType ptIdx1, IndexType ptIdx2)
    {
        KeyType upper = ptIdx1 < ptIdx2 ? ptIdx1 : ptIdx2;
        KeyType lower = ptIdx1 < ptIdx2 ? ptIdx2 : ptIdx1;
        KeyType key = (upper << BitShift) + lower;
        
        auto it = midpointIndices.find(key);
        if (it != midpointIndices.end())
        {
            return it->second;
        }
        
        Vector3 midPt;
        bx::vec3Add((float*)midPt,
            (const float*)vertices[ptIdx1],
            (const float*)vertices[ptIdx2]);
        bx::vec3Mul((float*)midPt, (const float*)midPt, 0.5f);
        IndexType midPtIdx = addVertex(midPt);
        midpointIndices.emplace(key, midPtIdx);
        return midPtIdx;
    }
    
    void addFace(IndexType v0, IndexType v1, IndexType v2)
    {
        indices.push_back(v0);
        indices.push_back(v1);
        indices.push_back(v2);
    }
    
    void subdivide()
    {
        if (indices.empty())
            return;
        
        IndexType offset = indices.size();
        
        for (IndexType i = 0; i < offset; i+=3)
        {
            IndexType a = addMiddlePoint(indices[i+0], indices[i+1]);
            IndexType b = addMiddlePoint(indices[i+1], indices[i+2]);
            IndexType c = addMiddlePoint(indices[i+2], indices[i+0]);
            
            addFace( indices[i+0], a, c );
            addFace( indices[i+1], b, a );
            addFace( indices[i+2], c, b );
            addFace( a, b, c );
        }
        
        //  erase the old faces.
        indices.erase(indices.begin(), indices.begin() + offset);
    }
};
        
unique_ptr<Mesh> createIcoSphere
(
    float radius,
    int subdividePasses,
    VertexTypes::Format vertexType,
    const Vector4& color,
    const Allocator& allocator
)
{
    const bgfx::VertexDecl& vertexDecl = VertexTypes::declaration(vertexType);
    
    if (vertexDecl.getStride()==0)
        return nullptr;
    
    //  Re: blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
    IcoSphereUtility<uint32_t, uint16_t, 16> utility(subdividePasses, allocator);
    
    const float kSqrt5 = 2.23606797749979f;
    float t = (radius + kSqrt5) * 0.5f;
    
    utility.addVertex({{ -1.f,    t,  0.f}});
    utility.addVertex({{  1.f,    t,  0.f}});
    utility.addVertex({{ -1.f,   -t,  0.f}});
    utility.addVertex({{  1.f,   -t,  0.f}});
    
    utility.addVertex({{  0.f, -1.f,    t}});
    utility.addVertex({{  0.f,  1.f,    t}});
    utility.addVertex({{  0.f, -1.f,   -t}});
    utility.addVertex({{  0.f,  1.f,   -t}});
    
    utility.addVertex({{    t,  0.f, -1.f}});
    utility.addVertex({{    t,  0.f,  1.f}});
    utility.addVertex({{   -t,  0.f, -1.f}});
    utility.addVertex({{   -t,  0.f,  1.f}});
    
    //  use clockwise faces for bgfx conformance
    //  faces of icosahedron
    utility.addFace(5, 11, 0);
    utility.addFace(1, 5, 0);
    utility.addFace(7, 1, 0);
    utility.addFace(10, 7, 0);
    utility.addFace(11, 10, 0);

    utility.addFace(9, 5, 1);
    utility.addFace(4, 11, 5);
    utility.addFace(2, 10, 11);
    utility.addFace(6, 7, 10);
    utility.addFace(8, 1, 7);

    utility.addFace(4, 9, 3);
    utility.addFace(2, 4, 3);
    utility.addFace(6, 2, 3);
    utility.addFace(8, 6, 3);
    utility.addFace(9, 8, 3);
    
    utility.addFace(5, 9, 4);
    utility.addFace(11, 4, 2);
    utility.addFace(10, 2, 6);
    utility.addFace(7, 6, 8);
    utility.addFace(1, 8, 9);
    
    //  recursion/subdivision pass
    for (int i = 0; i < subdividePasses; ++i)
    {
        utility.subdivide();
    }
    
    //  allocate packed buffers based on vertex declaration
    //
    uint32_t vertBufSize = vertexDecl.getSize((uint32_t)utility.vertices.size());
    const bgfx::Memory* vertexMemory = bgfx::alloc(vertBufSize);
    uint32_t idxBufSize =  (uint32_t)(utility.indices.size()*sizeof(decltype(utility)::index_type));
    const bgfx::Memory* indexMemory = bgfx::alloc(idxBufSize);
    
    for (int iv = 0; iv < utility.vertices.size(); ++iv)
    {
        if (vertexDecl.has(bgfx::Attrib::Position))
        {
            bgfx::vertexPack(utility.vertices[iv].comp, false,
                bgfx::Attrib::Position,
                vertexDecl,
                vertexMemory->data,
                iv);
        }
        if (vertexDecl.has(bgfx::Attrib::Color0))
        {
            bgfx::vertexPack(color.comp, true,
                bgfx::Attrib::Color0,
                vertexDecl,
                vertexMemory->data,
                iv);
        }
        /*
        if (vertexDecl.has(bgfx::Attrib::TexCoord0))
        {
        }
        */
    }
    
    CK_ASSERT(idxBufSize == indexMemory->size);
    memcpy(indexMemory->data, utility.indices.data(), idxBufSize);
    
    Allocator meshalloc(allocator);
    
    Matrix4 transform;
    bx::mtxIdentity(transform.comp);
    auto mesh = allocate_unique<Mesh>(meshalloc,
        vertexType,
        transform,
        vertexMemory, indexMemory,
        1, meshalloc);
    
    return mesh;
}

////////////////////////////////////////////////////////////////////////////////

Mesh::Mesh
(
    VertexTypes::Format format,
    const Matrix4& transform,
    const bgfx::Memory* vertexData,
    const bgfx::Memory* indexData,
    uint32_t elementCount,
    const Allocator& allocator
) :
    _format(format),
    _nodes(allocator)
{
    _nodes.reserve(elementCount);
    createNode(-1, transform, vertexData, indexData);
}

Mesh::~Mesh()
{
    for (auto& node : _nodes)
    {
        bgfx::destroyIndexBuffer(node.element.idxBufH);
        bgfx::destroyVertexBuffer(node.element.vertBufH);
    }
}

Mesh::Mesh(Mesh&& other) :
    _format(other._format),
    _nodes(std::move(other._nodes))
{
    other._format = VertexTypes::kInvalid;
}

Mesh& Mesh::operator=(Mesh&& other)
{
    _format = other._format;
    _nodes = std::move(other._nodes);
    
    other._format = VertexTypes::kInvalid;
    
    return *this;
}

int32_t Mesh::addElement
(
    int32_t parentIndex,
    const Matrix4& transform,
    const bgfx::Memory* vertexData,
    const bgfx::Memory* indexData
)
{
    if (parentIndex < 0 || parentIndex >= _nodes.size())
        return -1;

    int32_t thisIndex = createNode(parentIndex, transform, vertexData, indexData);
    if (thisIndex < 0)
        return -1;
    
    //  inject our node's index into our parent or sibling to fixup its location
    //  within the element tree
    
    auto& parent = _nodes[parentIndex];
    int32_t siblingIndex = parent.element.firstChildIdx;
    if (siblingIndex < 0)
    {
        parent.element.firstChildIdx = thisIndex;
    }
    else
    {
        while (_nodes[siblingIndex].element.nextSiblingIdx >= 0)
            siblingIndex = _nodes[siblingIndex].element.nextSiblingIdx;
        _nodes[siblingIndex].element.nextSiblingIdx = thisIndex;
    }
    
    return thisIndex;
}

int32_t Mesh::createNode(int32_t parentIndex, const Matrix4& transform,
     const bgfx::Memory* vertexData,
     const bgfx::Memory* indexData)
{
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
        vertexData,
        VertexTypes::declaration(_format)
    );
    if (!bgfx::isValid(vbh))
        return -1;
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(indexData);
    if (!bgfx::isValid(ibh))
    {
        bgfx::destroyVertexBuffer(vbh);
        return -1;
    }
    
    _nodes.emplace_back();
    auto& node = _nodes.back();
    node.element.parentIdx = parentIndex;
    node.element.transform = transform;
    node.element.vertBufH = vbh;
    node.element.idxBufH = ibh;
    node.element.firstChildIdx = -1;
    node.element.nextSiblingIdx = -1;
    
    return (int32_t)_nodes.size() - 1;
}
    
    }   // namespace gfx
}   // namespace cinek