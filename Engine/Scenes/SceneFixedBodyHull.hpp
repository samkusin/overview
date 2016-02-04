//
//  SceneFixedBodyHull.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/3/15.
//
//

#ifndef Overview_SceneFixedBodyHull_hpp
#define Overview_SceneFixedBodyHull_hpp

#include "SceneTypes.hpp"

#include <cinek/allocator.hpp>
#include <bullet/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>

#include <string>

namespace cinek {
    namespace ove {

/**
 *  @class  SceneFixedObject
 *  @brief  A Scene object for fixed, static objects (like architecture.)
 *
 *  Uses float value vertices to maintain compatibility with Recast, regardless
 *  of Bullet precision mode.  Fortunately btIndexedMesh supports this special
 *  case. 
 */
 
class SceneFixedBodyHull : public btTriangleIndexVertexArray
{
public:
    struct VertexIndexCount
    {
        int numFaces;
        int numVertices;
    };
    SceneFixedBodyHull();
    SceneFixedBodyHull(const std::string& name, const VertexIndexCount& initParams);
    virtual ~SceneFixedBodyHull();
    
    float* pullVertices(int triCount);
    int* pullFaceIndices(int faceCount);
    void finalize();
    
    const std::string& name() const { return _name; }

    int refCnt() const { return _refcnt; }
        
private:
    friend class SceneDataContext;
    int incRef() { return ++_refcnt; }
    int decRef() { return --_refcnt; }

private:
    Allocator _allocator;
    float* _vertexMemory;
    int* _indexMemory;
    VertexIndexCount _tail;
    VertexIndexCount _limit;
    
    std::string _name;
    int16_t _refcnt;
};


    } /* namespace ove */
} /* namespace cinek  */


#endif /* Overview_SceneObject_hpp */
