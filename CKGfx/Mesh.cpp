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

#include <array>

namespace cinek {
    namespace gfx {

template<typename KeyType, typename IndexType, int BitShift>
struct IcoSphereUtility
{
    using index_type = IndexType;
    using key_type = KeyType;
    
    unordered_map<KeyType, IndexType> midpointIndices;
    vector<std::array<IndexType, 7>> vertexAdjFaces;// [0]=faceCnt,[1-6]=faceIdx
    
    struct Face
    {
        IndexType idx[3];
    };
    
    vector<Vector3> vertices;
    vector<Face> faces;
    vector<Vector3> faceNormals;
    
    IcoSphereUtility(int passes, const Allocator& allocator) :
        midpointIndices(allocator),
        vertices(allocator),
        faces(allocator)
    {
        const uint32_t kNumVerts = 12 + 30 * passes*passes;
        vertices.reserve(kNumVerts);
        vertexAdjFaces.reserve(kNumVerts);
        
        uint32_t numFaces = 1;
        for (int i = 1; i <= passes; ++i)
            numFaces *= 4;
        
        numFaces *= 20;
        
        faceNormals.reserve( numFaces );
        
        //  We're using the same index vector for current indices
        //  and the new index vector created after a recursive/subdivide pass.
        //  So make room for additional indices by adding an extra pass.
        
        faces.reserve( (numFaces * 4) );
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
        faces.push_back({v0, v1, v2});
    }
    
    void subdivide()
    {
        if (faces.empty())
            return;
        
        auto offset = faces.size();
        
        for (auto i = 0; i < offset; ++i)
        {
            Face& face = faces[i];
            IndexType a = addMiddlePoint(face.idx[0], face.idx[1]);
            IndexType b = addMiddlePoint(face.idx[1], face.idx[2]);
            IndexType c = addMiddlePoint(face.idx[2], face.idx[0]);
            
            addFace( face.idx[0], a, c );
            addFace( face.idx[1], b, a );
            addFace( face.idx[2], c, b );
            addFace( a, b, c );
        }
        
        //  erase the old faces.
        faces.erase(faces.begin(), faces.begin() + offset);
    }
    
    void finalize()
    {
        vertexAdjFaces.clear();
        
        typename decltype(vertexAdjFaces)::value_type adjFaces = { 0, };
        vertexAdjFaces.assign(vertices.size(), adjFaces);

        faceNormals.clear();
        
        for (size_t i = 0; i < faces.size(); ++i)
        {
            Face& face = faces[i];
            // v0 * v1 = normal
            Vector3 v0, v1;
            auto i0 = face.idx[0], i1 = face.idx[1], i2 = face.idx[2];
            bx::vec3Sub(v0, vertices[i1], vertices[i0]);
            bx::vec3Sub(v1, vertices[i2], vertices[i0]);
            bx::vec3Norm(v0, v0);
            bx::vec3Norm(v1, v1);
            faceNormals.emplace_back();
            bx::vec3Cross(faceNormals.back(), v0, v1);
            
            IndexType faceIndex = (IndexType)faceNormals.size()-1;
            addFaceToVertex(faceIndex, i0);
            addFaceToVertex(faceIndex, i1);
            addFaceToVertex(faceIndex, i2);
        }
    }
    
    void addFaceToVertex(IndexType faceIndex, IndexType vertexIndex)
    {
        auto& adjFaces = vertexAdjFaces[vertexIndex];
        auto& faceCnt = adjFaces[0];
        CK_ASSERT(faceCnt+1 < adjFaces.size());
        ++faceCnt;
        adjFaces[faceCnt] = faceIndex;
    }
    
    Vector3 vertexNormal(int vertIdx)
    {
        Vector3 normal = {{ 0.f,0.f,0.f }};
        //  add all face normals attached to this vertex and normalize for
        //  the vertex normal
        auto& adjFaces = vertexAdjFaces[vertIdx];
        auto numFaces = adjFaces[0]+1;
        for (decltype(numFaces) iaf = 1; iaf < numFaces; ++iaf)
        {
            auto& faceNormal = faceNormals[adjFaces[iaf]];
            bx::vec3Add(normal, normal, faceNormal);
        }
        bx::vec3Norm(normal, normal);
        return normal;
    }
    
    
    void fixupEdgeUVs(vector<Vector2>& vertexUVs, vector<Vector3>& normals,
                      Face& face,
                      int index0, int index1)
    {
        //  if a boundary face, update the face with the corrected UV and
        //  duplicated vertex.
        //  we also need to generate a normal for any new vertices created
        
        //  do not fixup pole vertices - their UVs will be fixed up in another
        //  step
        auto& n0 = normals[face.idx[index0]];
        auto& n1 = normals[face.idx[index1]];
        if (n0.comp[1] == 1.f || n0.comp[1] == -1.f)
            return;
        if (n1.comp[1] == 1.f || n1.comp[1] == -1.f)
            return;
        
        auto& uv0 = vertexUVs[face.idx[index0]];
        auto& uv1 = vertexUVs[face.idx[index1]];
        auto uDiff = std::fabs(uv0.comp[0] - uv1.comp[0]);
        if (uDiff > 0.25f)
        {
            //  new vertex,normal and UV required - update the face with the
            //  new vert-index.
            if (uv0.comp[0] > uv1.comp[0])
            {
                vertexUVs.push_back(uv1);
                vertexUVs.back().comp[0] += 1.0f;
                vertices.push_back(vertices[face.idx[index1]]);
                normals.push_back(normals[face.idx[index1]]);
                face.idx[index1] = vertices.size()-1;
            }
            else
            {
                vertexUVs.push_back(uv0);
                vertexUVs.back().comp[0] += 1.0f;
                vertices.push_back(vertices[face.idx[index0]]);
                normals.push_back(normals[face.idx[index0]]);
                face.idx[index0] = vertices.size()-1;
            }
        }
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
    float t = (1.f + kSqrt5) * 0.5f;
    
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
    
    //  faces of icosahedron
    utility.addFace(5, 0, 11);
    utility.addFace(1, 0, 5);
    utility.addFace(7, 0, 1);
    utility.addFace(10, 0, 7);
    utility.addFace(11, 0, 10);

    utility.addFace(9, 1, 5);
    utility.addFace(4, 5, 11);
    utility.addFace(2, 11, 10);
    utility.addFace(6, 10, 7);
    utility.addFace(8, 7, 1);

    utility.addFace(4, 3, 9);
    utility.addFace(2, 3, 4);
    utility.addFace(6, 3, 2);
    utility.addFace(8, 3, 6);
    utility.addFace(9, 3, 8);
    
    utility.addFace(5, 4, 9);
    utility.addFace(11, 2, 4);
    utility.addFace(10, 6, 2);
    utility.addFace(7, 8, 6);
    utility.addFace(1, 9, 8);
    
    //  recursion/subdivision pass
    for (int i = 0; i < subdividePasses; ++i)
    {
        utility.subdivide();
    }
    
    utility.finalize();
    
    //  calculate normals and UVs, reserving extra space for duplicate face
    //  generation (re: UV wrapping)
    vector<Vector2> vertexUVs;
    vector<Vector3> normals;
    
    bool hasUVs = vertexDecl.has(bgfx::Attrib::TexCoord0);
    
    vertexUVs.reserve(utility.vertices.size() + utility.vertices.size()/4);
    normals.reserve(utility.vertices.size() + utility.vertices.size()/4);
    
    for (int iv = 0; iv < utility.vertices.size(); ++iv)
    {
        normals.emplace_back(utility.vertexNormal(iv));
        if (hasUVs)
        {
            const Vector3& normal = normals.back();
        
            //  invert our y normal coordinate - this is aesthetic choice since
            //  we want the top of the texture to match positive Y in 3dspace
            Vector2 uv;
            if (normal.comp[2] || normal.comp[0])
            {
                uv.comp[0] = atan2f(normal.comp[2], normal.comp[0])/(2*bx::pi) + 0.5f;
            }
            else
            {
                uv.comp[0] = 0.0f;
            }
            uv.comp[1] = 0.5f - asinf(normal.comp[1])/bx::pi;
            vertexUVs.emplace_back(uv);
        }
    }
    
    if (hasUVs)
    {
        //  Fixup UVs for faces that lie on or past the boundary Umax, Vmax.
        //  -Iterate through all faces to find such "boundary faces".
        //  -Create new vertices when needed so the face UVs correctly wrap the'
        //  sphere.
        //
        for (auto it = utility.faces.begin(); it != utility.faces.end(); ++it)
        {
            auto& face = *it;
            
            utility.fixupEdgeUVs(vertexUVs, normals, face, 0, 1);
            utility.fixupEdgeUVs(vertexUVs, normals, face, 1, 2);
            utility.fixupEdgeUVs(vertexUVs, normals, face, 2, 0);
            
        }
        
        bool yPosNormalFound = false;
        bool yNegNormalFound = false;
        for (auto it = utility.faces.begin(); it != utility.faces.end(); ++it)
        {
            auto& face = *it;
  
            //  find the pole vertices - they'll have +-1 Y axis normals
            //  create new verices
            for (int i = 0; i < 3; ++i)
            {
                auto& normal = normals[face.idx[i]];
                
                //  don't duplicate the first pole vertex found to prevent
                //  creating vertices that aren't used
                bool newVertex = false;
                if (normal.comp[1] == 1.0f)
                {
                    newVertex = yPosNormalFound;
                    yPosNormalFound = true;
                }
                else if (normal.comp[1] == -1.0f)
                {
                    newVertex = yNegNormalFound;
                    yNegNormalFound = true;
                }
                if (normal.comp[1] == 1.0f || normal.comp[1] == -1.0f)
                {
                    
                    if (newVertex)
                    {
                        vertexUVs.push_back(vertexUVs[face.idx[i]]);
                        utility.vertices.push_back(utility.vertices[face.idx[i]]);
                        normals.push_back(normal);
                        face.idx[i] = utility.vertices.size()-1;
                    }
                    //  our "new U" will be the average of the other two vertices
                    //  U values (http://sol.gfxile.net/sphere/)
                    float vertAU = vertexUVs[face.idx[(i+1)%3]].comp[0];
                    float vertBU = vertexUVs[face.idx[(i+2)%3]].comp[0];
                    vertexUVs[face.idx[i]].comp[0] = (vertAU+vertBU)*0.5f;
                    /*
                    printf("UV[%d]=(%.2f,%.2f), ", i,
                        vertexUVs[face.idx[i]].comp[0],
                        vertexUVs[face.idx[i]].comp[1]);
                    printf("UV[%d]=(%.2f,%.2f), ", (i+1)%3,
                        vertexUVs[face.idx[(i+1)%3]].comp[0],
                        vertexUVs[face.idx[(i+1)%3]].comp[1]);
                    printf("UV[%d]=(%.2f,%.2f) \n ", (i+2)%3,
                        vertexUVs[face.idx[(i+2)%3]].comp[0],
                        vertexUVs[face.idx[(i+2)%3]].comp[1]);
                    */
                    break;
                }
            }
        }
    }
    
    //  allocate packed buffers based on vertex declaration and generate our
    //  hardware buffers
    uint32_t vertBufSize = vertexDecl.getSize((uint32_t)utility.vertices.size());
    const bgfx::Memory* vertexMemory = bgfx::alloc(vertBufSize);
    uint32_t idxBufSize =  (uint32_t)(utility.faces.size()*sizeof(decltype(utility)::index_type)*3);
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
        if (vertexDecl.has(bgfx::Attrib::Normal))
        {
            bgfx::vertexPack(normals[iv], false,
                bgfx::Attrib::Normal,
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
        if (vertexDecl.has(bgfx::Attrib::TexCoord0))
        {
            bgfx::vertexPack(vertexUVs[iv], false,
                bgfx::Attrib::TexCoord0,
                vertexDecl,
                vertexMemory->data,
                iv);
        }
    }
    
    CK_ASSERT(idxBufSize == indexMemory->size);
    auto indexMemoryPtr = reinterpret_cast<decltype(utility)::index_type*>(indexMemory->data);
    for (auto& face : utility.faces)
    {
        indexMemoryPtr[0] = face.idx[0];
        indexMemoryPtr[1] = face.idx[1];
        indexMemoryPtr[2] = face.idx[2];
        indexMemoryPtr += 3;
    }
    
    Allocator meshalloc(allocator);
    
    Matrix4 transform;
    bx::mtxScale(transform.comp, radius, radius, radius);
    auto mesh = allocate_unique<Mesh>(meshalloc,
        vertexType,
        transform,
        vertexMemory, indexMemory,
        1, meshalloc);
    
    return mesh;
}

Vector3 cubicMapReflectVector(const Vector3& normal, const Vector3& v)
{
    Vector3 r;
    float dotNV2 = 2 * bx::vec3Dot(normal, v);
    bx::vec3Mul(r, r, dotNV2);
    bx::vec3Sub(r, r, v);
    return r;
}

unique_ptr<Mesh> createCube
(
    float radius,
    VertexTypes::Format vertexType,
    const Vector4& color,
    const Allocator& allocator
)
{
    const bgfx::VertexDecl& vertexDecl = VertexTypes::declaration(vertexType);
    
    if (vertexDecl.getStride()==0)
        return nullptr;
 
    struct Face
    {
        int idx[3];
    };
 
    std::array<Vector3, 24> vertices;   // cubic uvs requires dup vertices
    std::array<Vector3, 24> normals;
    int numVertices = 0;
    
    //  create base set of vertices, faces and normals.
    vertices[0] = {{  0.5f,  0.5f, -0.5f }};    // +X
    vertices[1] = {{  0.5f,  0.5f,  0.5f }};
    vertices[2] = {{  0.5f, -0.5f,  0.5f }};
    vertices[3] = {{  0.5f, -0.5f, -0.5f }};
    vertices[4] = {{  0.5f,  0.5f, -0.5f }};    // -X
    vertices[5] = {{  0.5f,  0.5f,  0.5f }};
    vertices[6] = {{  0.5f, -0.5f,  0.5f }};
    vertices[7] = {{  0.5f, -0.5f, -0.5f }};
    numVertices = 8;
    
    if (vertexDecl.has(bgfx::Attrib::Normal))
    {
        // since vertices are equally distant from the origin...
        for (int i = 0; i < 8; ++i)
        {
            bx::vec3Norm(normals[i], vertices[i]);
        }
    }
    
    //  faces are ordered, +X,-X,+Y,-Y,+Z,-Z
    std::array<Face, 12> faces;         // 6 sides, 2 tris per side
 
    faces[0].idx[0] = 0;                // +X face
    faces[0].idx[1] = 2;
    faces[0].idx[2] = 3;
    faces[1].idx[0] = 0;
    faces[1].idx[1] = 1;
    faces[1].idx[2] = 2;
    
    faces[2].idx[0] = 4;                // -X face
    faces[2].idx[1] = 5;
    faces[2].idx[2] = 6;
    faces[3].idx[0] = 4;
    faces[3].idx[1] = 6;
    faces[3].idx[2] = 7;
    
    faces[4].idx[0] = 0;                // +Y face
    faces[4].idx[1] = 4;
    faces[4].idx[2] = 7;
    faces[5].idx[0] = 0;
    faces[5].idx[1] = 7;
    faces[5].idx[2] = 1;
    
    faces[6].idx[0] = 5;                // -Y face
    faces[6].idx[1] = 3;
    faces[6].idx[2] = 2;
    faces[7].idx[0] = 5;
    faces[7].idx[1] = 2;
    faces[7].idx[2] = 6;
    
    faces[8].idx[0] = 1;                // +Z face
    faces[8].idx[1] = 7;
    faces[8].idx[2] = 6;
    faces[9].idx[0] = 1;
    faces[9].idx[1] = 6;
    faces[9].idx[2] = 2;
    
    faces[10].idx[0] = 4;                // -Z face
    faces[10].idx[1] = 3;
    faces[10].idx[2] = 5;
    faces[11].idx[0] = 4;
    faces[11].idx[1] = 0;
    faces[11].idx[2] = 3;
    
    //  add uvs and duplicate vertices as needed
    /*
    std::array<Vector2, 24> texUVs;
    
    if (vertexDecl.has(bgfx::Attrib::TexCoord0))
    {
        Vector3 faceNorm;
        faceNorm.from(1,0,0);
        int vidx = faces[0].idx[0];
        Vector3 r = cubicMapReflectVector(faceNorm, vertices[vidx]);
        //  x-major
        if (r.x() >= r.y() && r.x() >= r.z())
        {
            float sc
        }
        
    }
    */
    
    uint32_t vertBufSize = vertexDecl.getSize(numVertices);
    const bgfx::Memory* vertexMemory = bgfx::alloc(vertBufSize);
    uint32_t idxBufSize =  (uint32_t)(12*sizeof(uint16_t)*3);
    const bgfx::Memory* indexMemory = bgfx::alloc(idxBufSize);
    
    for (int iv = 0; iv < numVertices; ++iv)
    {
        if (vertexDecl.has(bgfx::Attrib::Position))
        {
            bgfx::vertexPack(vertices[iv].comp, false,
                bgfx::Attrib::Position,
                vertexDecl,
                vertexMemory->data,
                iv);
        }
        if (vertexDecl.has(bgfx::Attrib::Normal))
        {
            bgfx::vertexPack(normals[iv], false,
                bgfx::Attrib::Normal,
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
            bgfx::vertexPack(vertexUVs[iv], false,
                bgfx::Attrib::TexCoord0,
                vertexDecl,
                vertexMemory->data,
                iv);
        }
        */
    }
    
    CK_ASSERT(idxBufSize == indexMemory->size);
    auto indexMemoryPtr = reinterpret_cast<uint16_t*>(indexMemory->data);
    for (auto& face : faces)
    {
        indexMemoryPtr[0] = face.idx[0];
        indexMemoryPtr[1] = face.idx[1];
        indexMemoryPtr[2] = face.idx[2];
        indexMemoryPtr += 3;
    }
    
    Matrix4 transform;
    bx::mtxScale(transform.comp, radius, radius, radius);
    
    Allocator meshalloc(allocator);
    
    auto mesh = allocate_unique<Mesh>(meshalloc,
        vertexType,
        transform,
        vertexMemory, indexMemory,
        1, meshalloc);

    return mesh;
}

////////////////////////////////////////////////////////////////////////////////

Mesh::Mesh() :
    _format(VertexTypes::kInvalid)
{
}

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