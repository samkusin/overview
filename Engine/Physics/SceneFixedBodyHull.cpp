//
//  SceneFixedBodyHull.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/3/15.
//
//

#include "SceneFixedBodyHull.hpp"
#include "Engine/Debug.hpp"

namespace cinek {
    namespace ove {

/**
 *  @class  SceneFixedObject
 *  @brief  A Scene object for fixed, static objects (like architecture.)
 */
 
SceneFixedBodyHull::SceneFixedBodyHull() :
    _vertexMemory(nullptr), _indexMemory(nullptr),
    _tail {}, _limit {},
    _refcnt(0)
{
}

SceneFixedBodyHull::SceneFixedBodyHull
(
    const std::string& name,
    const VertexIndexCount& initParams
) :
    SceneFixedBodyHull()
{
    _name = name;
    _vertexMemory = reinterpret_cast<float*>(
        _allocator.alloc(initParams.numVertices * sizeof(float) * 3)
    );
    _indexMemory = reinterpret_cast<int*>(
        _allocator.alloc(initParams.numFaces * sizeof(int) * 3)
    );
    _limit = initParams;
}

SceneFixedBodyHull::~SceneFixedBodyHull()
{
    if (_indexMemory) {
        _allocator.free(_indexMemory);
    }
    if (_vertexMemory) {
        _allocator.free(_vertexMemory);
    }
}
    
float* SceneFixedBodyHull::pullVertices(int count)
{
    CK_ASSERT_RETURN_VALUE(_tail.numVertices + count <= _limit.numVertices, nullptr);
    float* p = _vertexMemory + _tail.numVertices*3;
    _tail.numVertices += count;
    return p;
}

int* SceneFixedBodyHull::pullTriangleIndices(int count)
{
    CK_ASSERT_RETURN_VALUE(_tail.numFaces + count <= _limit.numFaces, nullptr);
    int* p = _indexMemory + _tail.numFaces*3;
    _tail.numFaces += count;
    return p;
}

void SceneFixedBodyHull::finalize()
{
    btIndexedMesh mesh;
    mesh.m_numTriangles = _tail.numFaces;
    mesh.m_numVertices = _tail.numVertices;
    mesh.m_triangleIndexBase = reinterpret_cast<const unsigned char*>(_indexMemory);
    mesh.m_vertexBase = reinterpret_cast<const unsigned char*>(_vertexMemory);
    mesh.m_triangleIndexStride = 3 * sizeof(int);
    mesh.m_vertexStride = sizeof(float) * 3;
    mesh.m_vertexType = PHY_FLOAT;          // keep as float for Recast compata
    btTriangleIndexVertexArray::addIndexedMesh(mesh, PHY_INTEGER);
}

    } /* namespace ove */
} /* namespace cinek  */
