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
    _vertexMemory = reinterpret_cast<btVector3*>(
        _allocator.alloc(initParams.numVertices * sizeof(btVector3))
    );
    _indexMemory = reinterpret_cast<uint16_t*>(
        _allocator.alloc(initParams.numFaces * sizeof(uint16_t) * 3)
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
    
btVector3* SceneFixedBodyHull::pullVertices(uint16_t count)
{
    CK_ASSERT_RETURN_VALUE(_tail.numVertices + count <= _limit.numVertices, nullptr);
    btVector3* p = _vertexMemory + _tail.numVertices;
    _tail.numVertices += count;
    return p;
}

uint16_t* SceneFixedBodyHull::pullFaceIndices(uint16_t count)
{
    CK_ASSERT_RETURN_VALUE(_tail.numFaces + count <= _limit.numFaces, nullptr);
    uint16_t* p = _indexMemory + _tail.numFaces*3;
    _tail.numFaces += count;
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
    mesh.m_vertexStride = sizeof(btVector3);
    
    btTriangleIndexVertexArray::addIndexedMesh(mesh);
}

    } /* namespace ove */
} /* namespace cinek  */
