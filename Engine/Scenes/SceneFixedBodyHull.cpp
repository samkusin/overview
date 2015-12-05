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
    _tail {}, _limit {}
{
}

SceneFixedBodyHull::SceneFixedBodyHull(const VertexIndexCount& initParams) :
    SceneFixedBodyHull()
{
    _vertexMemory = reinterpret_cast<btVector3*>(
        _allocator.alloc(initParams.numVertices * sizeof(btVector3))
    );
    _indexMemory = reinterpret_cast<uint16_t*>(
        _allocator.alloc(initParams.numFaces * sizeof(uint16_t) * 3)
    );
    _limit = initParams;
}
    
btVector3* SceneFixedBodyHull::pullVertices(uint16_t count)
{
    CK_ASSERT_RETURN_VALUE(_tail.numVertices + count <= _limit.numVertices, nullptr);
    btVector3* p = _vertexMemory;
    _vertexMemory += count;
    return p;
}

uint16_t* SceneFixedBodyHull::pullFaceIndices(uint16_t count)
{
    CK_ASSERT_RETURN_VALUE(_tail.numFaces + count <= _limit.numFaces, nullptr);
    uint16_t* p = _indexMemory;
    _indexMemory += count*3;
    return p;
}

void SceneFixedBodyHull::finialize()
{
    btIndexedMesh mesh;
    mesh.m_indexType = PHY_SHORT;
    mesh.m_numTriangles = _tail.numFaces;
    mesh.m_numVertices = _tail.numVertices;
    mesh.m_triangleIndexBase = reinterpret_cast<const unsigned char*>(_indexMemory);
    mesh.m_vertexBase = reinterpret_cast<const unsigned char*>(_vertexMemory);
    mesh.m_triangleIndexStride = 3 * sizeof(uint16_t);
    mesh.m_vertexStride = 3 * sizeof(btVector3);
    
    btTriangleIndexVertexArray::addIndexedMesh(mesh);
}

    } /* namespace ove */
} /* namespace cinek  */
