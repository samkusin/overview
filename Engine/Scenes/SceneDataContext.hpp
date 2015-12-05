//
//  SceneDataContext.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/3/15.
//
//

#ifndef Overview_SceneContextData_hpp
#define Overview_SceneContextData_hpp

#include "Engine/EngineTypes.hpp"

#include "SceneFixedBodyHull.hpp"

#include <cinek/objectpool.hpp>

#include <bullet/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>

#include <vector>
#include <string>

namespace cinek {
    namespace ove {
    
class SceneDataContext
{
    CK_CLASS_NON_COPYABLE(SceneDataContext);
    
public:
    struct InitParams
    {
        uint32_t numTriMeshShapes;
        uint32_t numRigidBodies;
    };
    SceneDataContext() = default;
    SceneDataContext(const InitParams& params);
    
    //  resources shared among multiple bodies
    SceneFixedBodyHull* allocateFixedBodyHull
    (
        const SceneFixedBodyHull::VertexIndexCount& counts
    );
    void freeFixedBodyHull(SceneFixedBodyHull* hull);
    SceneTriangleMeshShape* allocateTriangleMeshShape
    (
        SceneFixedBodyHull* hull,
        std::string name
    );
    SceneTriangleMeshShape* triangleMeshShapeAcquire(const std::string& name);
    void tringleMeshShapeRelease(SceneTriangleMeshShape* shape);
    
    //  body
    SceneBody* allocateBody(const btRigidBody::btRigidBodyConstructionInfo& info,
                            Entity entity);
    void releaseBody(SceneBody* body);
    
private:
    ObjectPool<SceneFixedBodyHull> _triMeshPool;
    ObjectPool<SceneTriangleMeshShape> _triMeshShapePool;
    ObjectPool<SceneBody> _rigidBodyPool;
    
    std::vector<SceneTriangleMeshShape*> _triMeshShapes;
};
    
    } /* namespace ove */
} /* namespace cinek */


#endif /* Overview_SceneFixedBodyFactory_hpp */
