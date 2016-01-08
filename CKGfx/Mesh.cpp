//
//  Mesh.cpp
//  Overview
//
//  Created by Samir Sinha on 4/27/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "Mesh.hpp"

#include <cinek/debug.h>
#include <cinek/map.hpp>

#include <bx/fpumath.h>

#include <vector>
#include <array>

namespace cinek {
    namespace gfx {


namespace MeshBuilder
{
    BuilderState& BuilderState::position(const Vector3& pos)
    {
        return vertex(pos.comp, bgfx::Attrib::Position);
    }
    
    BuilderState& BuilderState::normal(const Vector3& normal)
    {
        return vertex(normal.comp, bgfx::Attrib::Normal);
    }
    
    BuilderState& BuilderState::uv2(const Vector2& uv)
    {
        return vertex(uv.comp, bgfx::Attrib::TexCoord0);
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
        vertex(indices.comp, bgfx::Attrib::Indices);
        vertex(weights.comp, bgfx::Attrib::Weight);
        
        weightStackIdx = 0;
        return *this;
    }
    
    
    BuilderState& BuilderState::vertex
    (
        const float* comp,
        bgfx::Attrib::Enum attrib
    )
    {
        CK_ASSERT(vertexCount < vertexLimit);
        bool valid = vertexDecl->has(attrib);
        if (valid) {
            bgfx::vertexPack(comp, false, attrib,
                             *vertexDecl,
                             vertexMemory->data,
                             vertexCount);
        }
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
        CK_ASSERT(indexCount + 3 <= indexLimit);
        CK_ASSERT(indexType == VertexTypes::kIndex16);
        uint16_t* ptr = reinterpret_cast<uint16_t*>(indexMemory->data) + indexCount;
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
        CK_ASSERT(indexCount + 3 <= indexLimit);
        CK_ASSERT(indexType == VertexTypes::kIndex32);
        uint32_t* ptr = reinterpret_cast<uint32_t*>(indexMemory->data) + indexCount;
        ptr[0] = i0;
        ptr[1] = i1;
        ptr[2] = i2;
        indexCount += 3;
        return *this;
    }

    BuilderState& create(BuilderState& state)
    {
        CK_ASSERT_RETURN_VALUE(state.indexType != VertexTypes::kIndex0, state);
        
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
        
        return state;
    }

}


template<typename KeyType, typename IndexType, int BitShift>
struct IcoSphereUtility
{
    using index_type = IndexType;
    using key_type = KeyType;
    
    unordered_map<KeyType, IndexType> midpointIndices;
    std::vector<std::array<IndexType, 7>> vertexAdjFaces;// [0]=faceCnt,[1-6]=faceIdx
    
    struct Face
    {
        IndexType idx[3];
    };
    
    std::vector<Vector3> vertices;
    std::vector<Face> faces;
    std::vector<Vector3> faceNormals;
    
    IcoSphereUtility(int passes)
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
        Vector3 normal = { 0.f,0.f,0.f };
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
    
    
    void fixupEdgeUVs(std::vector<Vector2>& vertexUVs, std::vector<Vector3>& normals,
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
        
Mesh createIcoSphere
(
    float radius,
    int subdividePasses,
    VertexTypes::Format vertexType
)
{
    const bgfx::VertexDecl& vertexDecl = VertexTypes::declaration(vertexType);
    
    if (vertexDecl.getStride()==0)
        return Mesh();
    
    //  Re: blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
    IcoSphereUtility<uint32_t, uint16_t, 16> utility(subdividePasses);
    
    const float kSqrt5 = 2.23606797749979f;
    float t = (1.f + kSqrt5) * 0.5f;
    
    utility.addVertex({ -1.f,    t,  0.f});
    utility.addVertex({  1.f,    t,  0.f});
    utility.addVertex({ -1.f,   -t,  0.f});
    utility.addVertex({  1.f,   -t,  0.f});
    
    utility.addVertex({  0.f, -1.f,    t});
    utility.addVertex({  0.f,  1.f,    t});
    utility.addVertex({  0.f, -1.f,   -t});
    utility.addVertex({  0.f,  1.f,   -t});
    
    utility.addVertex({    t,  0.f, -1.f});
    utility.addVertex({    t,  0.f,  1.f});
    utility.addVertex({   -t,  0.f, -1.f});
    utility.addVertex({   -t,  0.f,  1.f});
    
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
    std::vector<Vector2> vertexUVs;
    std::vector<Vector3> normals;
    
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
    MeshBuilder::BuilderState meshBuilder;
    meshBuilder.vertexDecl = &VertexTypes::declaration(vertexType);
    meshBuilder.vertexLimit = (uint32_t)utility.vertices.size();
    meshBuilder.indexLimit = (uint32_t)utility.faces.size() * 3;
    meshBuilder.indexType = VertexTypes::kIndex16;
    
    MeshBuilder::create(meshBuilder);
    
    for (int iv = 0; iv < utility.vertices.size(); ++iv) {
        meshBuilder.position(utility.vertices[iv])
                   .normal(normals[iv])
                   .uv2(vertexUVs[iv])
                   .next();
    }
    
    for (auto& face : utility.faces)
    {
        meshBuilder.triangle(face.idx[0], face.idx[1], face.idx[2]);
    }
    
    Mesh mesh(vertexType, meshBuilder.indexType,
              meshBuilder.vertexMemory,
              meshBuilder.indexMemory);
    
    
    
    return mesh;
}


Mesh createQuad
(
    float scale,
    VertexTypes::Format vertexType
)
{
    const bgfx::VertexDecl& vertexDecl = VertexTypes::declaration(vertexType);
    
    //  allocate packed buffers based on vertex declaration and generate our
    //  hardware buffers
    MeshBuilder::BuilderState meshBuilder;
    meshBuilder.vertexDecl = &VertexTypes::declaration(vertexType);
    meshBuilder.vertexLimit = 4;
    meshBuilder.indexLimit = 6;
    meshBuilder.indexType = VertexTypes::kIndex16;
    
    MeshBuilder::create(meshBuilder);
    
    //  CCW winding order
    const Vector3 verts[] = {
        { -scale, -scale, 0.0f },
        { scale, -scale, 0.0f },
        { scale, scale, 0.0f },
        { -scale, scale, 0.0 }
    };
    
    const Vector2 vertexUVs[] = {
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 1.0f, 1.0f },
        { 0.0f, 1.0f }
    };
    
    for (int iv = 0; iv < 4; ++iv) {
        meshBuilder.position(verts[iv]);
        
        if (vertexDecl.has(bgfx::Attrib::Normal)) {
            meshBuilder.normal({ 0.0f, 0.0f, -1.0f });
        }
        if (vertexDecl.has(bgfx::Attrib::TexCoord0)) {
            meshBuilder.uv2(vertexUVs[iv]);
        }
        
        meshBuilder.next();
    }
    
    meshBuilder.triangle<uint16_t>(0, 1, 2);
    meshBuilder.triangle<uint16_t>(3, 0, 2);
    
    
    Mesh mesh(vertexType, meshBuilder.indexType,
              meshBuilder.vertexMemory,
              meshBuilder.indexMemory);
    

    return mesh;
}


////////////////////////////////////////////////////////////////////////////////

Mesh::Mesh() :
    _format(VertexTypes::kInvalid),
    _indexType(VertexTypes::Index::kIndex0),
    _vertBufH(BGFX_INVALID_HANDLE),
    _idxBufH(BGFX_INVALID_HANDLE)
{
}

Mesh::Mesh
(
    VertexTypes::Format format,
    VertexTypes::Index indexType,
    const bgfx::Memory* vertexData,
    const bgfx::Memory* indexData
) :
    _format(format),
    _indexType(indexType),
    _vertBufH(BGFX_INVALID_HANDLE),
    _idxBufH(BGFX_INVALID_HANDLE)
{
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
        vertexData,
        VertexTypes::declaration(_format)
    );
    if (!bgfx::isValid(vbh))
        return;
    
    uint16_t indexFlags = BGFX_BUFFER_NONE;
    if (indexType == VertexTypes::kIndex32)
        indexFlags |= BGFX_BUFFER_INDEX32;
    
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(indexData, indexFlags);
    if (!bgfx::isValid(ibh))
    {
        bgfx::destroyVertexBuffer(vbh);
        return;
    }
    _vertBufH = vbh;
    _idxBufH = ibh;
}

Mesh::~Mesh()
{
    if (bgfx::isValid(_idxBufH)) {
        bgfx::destroyIndexBuffer(_idxBufH);
    }
    if (bgfx::isValid(_vertBufH)) {
        bgfx::destroyVertexBuffer(_vertBufH);
    }
}

Mesh::Mesh(Mesh&& other) :
    _format(other._format),
    _indexType(other._indexType),
    _vertBufH(other._vertBufH),
    _idxBufH(other._idxBufH)
{
    other._format = VertexTypes::kInvalid;
    other._indexType = VertexTypes::Index::kIndex0;
    other._vertBufH = BGFX_INVALID_HANDLE;
    other._idxBufH = BGFX_INVALID_HANDLE;
}

Mesh& Mesh::operator=(Mesh&& other)
{
    _format = other._format;
    _indexType = other._indexType;
    _vertBufH = other._vertBufH;
    _idxBufH = other._idxBufH;
    
    other._format = VertexTypes::kInvalid;
    other._indexType = VertexTypes::kIndex0;
    other._vertBufH = BGFX_INVALID_HANDLE;
    other._idxBufH = BGFX_INVALID_HANDLE;
    
    return *this;
}    
    }   // namespace gfx
}   // namespace cinek