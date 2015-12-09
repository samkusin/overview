//
//  SceneFixedBodyHull.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/3/15.
//
//

#ifndef Overview_SceneFixedBodyHull_hpp
#define Overview_SceneFixedBodyHull_hpp

#include "Engine/EngineTypes.hpp"

#include <cinek/allocator.hpp>
#include <bullet/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>

#include <string>

namespace cinek {
    namespace ove {

/**
 *  @class  SceneFixedObject
 *  @brief  A Scene object for fixed, static objects (like architecture.)
 */
 
class SceneFixedBodyHull : public btTriangleIndexVertexArray
{
public:
    struct VertexIndexCount
    {
        uint16_t numFaces;
        uint16_t numVertices;
    };
    SceneFixedBodyHull();
    SceneFixedBodyHull(const std::string& name, const VertexIndexCount& initParams);
    virtual ~SceneFixedBodyHull();
    
    btVector3* pullVertices(uint16_t triCount);
    uint16_t* pullFaceIndices(uint16_t faceCount);
    void finialize();
    
    const std::string& name() const { return _name; }

    int refCnt() const { return _refcnt; }
        
private:
    friend class SceneDataContext;
    int incRef() { return ++_refcnt; }
    int decRef() { return --_refcnt; }

private:
    Allocator _allocator;
    btVector3* _vertexMemory;
    uint16_t* _indexMemory;
    VertexIndexCount _tail;
    VertexIndexCount _limit;
    
    std::string _name;
    int16_t _refcnt;
};


    } /* namespace ove */
} /* namespace cinek  */


#endif /* Overview_SceneObject_hpp */
