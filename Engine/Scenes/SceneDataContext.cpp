//
//  SceneFixedBodyFactory.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/3/15.
//
//

#include "SceneDataContext.hpp"
#include "SceneObject.hpp"

#include <cinek/objectpool.inl>

namespace cinek {
    
    template class ObjectPool<ove::SceneFixedBodyHull>;
    template class ObjectPool<btBvhTriangleMeshShape>;
    template class ObjectPool<btCylinderShape>;
    template class ObjectPool<btBoxShape>;
    template class ObjectPool<btCompoundShape>;
    template class ObjectPool<btRigidBody>;
    template class ObjectPool<ove::SceneMotionState>;
    
    namespace ove {
    
SceneDataContext::SceneDataContext(const InitParams& params) :
    _triMeshPool(params.numTriMeshShapes),
    _triMeshShapePool(params.numTriMeshShapes),
    _cylinderShapePool(params.numCylinderShapes),
    _boxShapePool(params.numBoxShapes),
    _compoundShapePool(params.numBoxShapes + params.numCylinderShapes),
    _rigidBodyPool(params.numRigidBodies),
    _motionStatesPool(params.numRigidBodies)
{
    _fixedBodyHulls.reserve(params.numTriMeshShapes);
}

SceneFixedBodyHull* SceneDataContext::allocateFixedBodyHull
(
    const SceneFixedBodyHull::VertexIndexCount& counts,
    const std::string& name
)
{
    return _triMeshPool.construct(name, counts);
}

SceneFixedBodyHull* SceneDataContext::acquireFixedBodyHull
(
    const std::string& name
)
{
    auto it = std::lower_bound(_fixedBodyHulls.begin(), _fixedBodyHulls.end(), name,
        [](const SceneFixedBodyHull* obj, const std::string& name) -> bool {
            return obj->name() < name;
        });
    if (it == _fixedBodyHulls.end() || (*it)->name() != name)
        return nullptr;
    
    (*it)->incRef();
    return *it;
}

void SceneDataContext::releaseFixedBodyHull(SceneFixedBodyHull* hull)
{
    if (!hull)
        return;
    auto it = std::lower_bound(_fixedBodyHulls.begin(), _fixedBodyHulls.end(), hull->name(),
        [](const SceneFixedBodyHull* obj, const std::string& name) -> bool {
            return obj->name() < name;
        });
    if (it == _fixedBodyHulls.end() || (*it)->name() != hull->name())
        return;
    
    auto obj = *it;
    obj->decRef();
    if (obj->refCnt() <= 0) {
        _fixedBodyHulls.erase(it);
        _triMeshPool.destruct(obj);
    }
}


btBvhTriangleMeshShape* SceneDataContext::allocateTriangleMeshShape
(
    SceneFixedBodyHull* hull,
    const btVector3& scale
)
{
    btBvhTriangleMeshShape* triMeshShape = _triMeshShapePool.construct(hull, false);
    triMeshShape->setLocalScaling(scale);
    return triMeshShape;
}

btCompoundShape* SceneDataContext::allocateBoxShape
(
    const btVector3& halfDims,
    const btTransform& localTransform
)
{
    btCompoundShape* compShape = _compoundShapePool.construct(true, 1);
    if (compShape) {
        btBoxShape* shape = _boxShapePool.construct(halfDims);
        compShape->addChildShape(localTransform, shape);
    }
    return compShape;
}

btCompoundShape* SceneDataContext::allocateCylinderShape
(
    const btVector3& halfDims,
    const btTransform& localTransform
)
{
    btCompoundShape* compShape = _compoundShapePool.construct(true, 1);
    if (compShape) {
        btCylinderShape* shape = _cylinderShapePool.construct(halfDims);
        compShape->addChildShape(localTransform, shape);
    }
    return compShape;
}

void SceneDataContext::freeShape(btCollisionShape* collisionShape)
{
    switch (collisionShape->getShapeType())
    {
    case TRIANGLE_MESH_SHAPE_PROXYTYPE: {
        btBvhTriangleMeshShape* shape = static_cast<btBvhTriangleMeshShape*>(collisionShape);
        releaseFixedBodyHull(static_cast<SceneFixedBodyHull*>(shape->getMeshInterface()));
        _triMeshShapePool.destruct(shape);
    }
    break;
    case COMPOUND_SHAPE_PROXYTYPE: {
        btCompoundShape* compShape = static_cast<btCompoundShape*>(collisionShape);
        for (int i = compShape->getNumChildShapes(); i > 0; --i) {
            btCollisionShape* shape = compShape->getChildShape(i-1);
            freeShape(shape);
            compShape->removeChildShapeByIndex(i-1);
        }
    }
    break;
    case BOX_SHAPE_PROXYTYPE: {
        btBoxShape* shape = static_cast<btBoxShape*>(collisionShape);
        _boxShapePool.destruct(shape);
    }
    break;
    case CYLINDER_SHAPE_PROXYTYPE: {
        btCylinderShape* shape = static_cast<btCylinderShape*>(collisionShape);
        _cylinderShapePool.destruct(shape);
    }
    break;
    default:
        //  unsupported shape type
        CK_ASSERT(false);
        break;
    }
    
}

btRigidBody* SceneDataContext::allocateBody
(
    const SceneBodyInitParams& info,
    gfx::NodeHandle gfxNodeHandle
)
{
    SceneMotionState* motionState = nullptr;
    if (gfxNodeHandle) {
        motionState = allocateMotionState(gfxNodeHandle);
    }
    btRigidBody::btRigidBodyConstructionInfo btInfo(info.mass,
        motionState,
        info.collisionShape);
    
    auto obj = _rigidBodyPool.construct(btInfo);
    return obj;
}

void SceneDataContext::freeBody(btRigidBody* body)
{
    SceneMotionState* state = static_cast<SceneMotionState*>(body->getMotionState());
    if (state) {
        body->setMotionState(nullptr);
        freeMotionState(state);
    }
    _rigidBodyPool.destruct(body);
}

SceneMotionState* SceneDataContext::allocateMotionState(gfx::NodeHandle h)
{
    return _motionStatesPool.construct(h);
}

void SceneDataContext::freeMotionState(SceneMotionState* state)
{
    _motionStatesPool.destruct(state);
}
    

    }
}