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
#include "SceneMotionState.hpp"

#include <cinek/objectpool.hpp>

#include <bullet/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <bullet/BulletCollision/CollisionShapes/btBoxShape.h>
#include <bullet/BulletCollision/CollisionShapes/btCylinderShape.h>
#include <bullet/BulletCollision/CollisionShapes/btCompoundShape.h>
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
        uint32_t numCylinderShapes;
        uint32_t numBoxShapes;
        uint32_t numRigidBodies;
    };
    SceneDataContext() = default;
    SceneDataContext(const InitParams& params);
    
    //  resources shared among multiple bodies
    SceneFixedBodyHull* allocateFixedBodyHull
    (
        const SceneFixedBodyHull::VertexIndexCount& counts,
        const std::string& name
    );
    SceneFixedBodyHull* acquireFixedBodyHull(const std::string& name);
    void releaseFixedBodyHull(SceneFixedBodyHull* hull);
    
    btBvhTriangleMeshShape* allocateTriangleMeshShape
    (
        SceneFixedBodyHull* hull,
        const btVector3& scale
    );
    
    btCompoundShape* allocateBoxShape(const btVector3& halfDims,
        const btTransform& localTransform);
    btCompoundShape* allocateCylinderShape(const btVector3& halfDims,
        const btTransform& localTransform);
    void freeShape(btCollisionShape* shape);
    
    //  body
    struct SceneBodyInitParams
    {
        btCollisionShape* collisionShape;
        float mass;
    };
    btRigidBody* allocateBody
    (
        const SceneBodyInitParams& info,
        gfx::NodeHandle gfxNodeHandle
    );
    void freeBody(btRigidBody* body);
    
    SceneMotionState* allocateMotionState(gfx::NodeHandle h);
    void freeMotionState(SceneMotionState* state);
    
private:
    ObjectPool<SceneFixedBodyHull> _triMeshPool;
    ObjectPool<btBvhTriangleMeshShape> _triMeshShapePool;
    ObjectPool<btCylinderShape> _cylinderShapePool;
    ObjectPool<btBoxShape> _boxShapePool;
    ObjectPool<btCompoundShape> _compoundShapePool;
    ObjectPool<btRigidBody> _rigidBodyPool;
    ObjectPool<SceneMotionState> _motionStatesPool;
    
    std::vector<SceneFixedBodyHull*> _fixedBodyHulls;
};
    
    } /* namespace ove */
} /* namespace cinek */


#endif /* Overview_SceneFixedBodyFactory_hpp */
