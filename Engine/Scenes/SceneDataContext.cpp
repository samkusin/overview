//
//  SceneFixedBodyFactory.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/3/15.
//
//

#include "SceneDataContext.hpp"

#include <cinek/objectpool.inl>

namespace cinek {
    
    template class ObjectPool<ove::SceneFixedBodyHull>;
    template class ObjectPool<btBvhTriangleMeshShape>;
    template class ObjectPool<btCylinderShape>;
    template class ObjectPool<btBoxShape>;
    template class ObjectPool<btCompoundShape>;
    template class ObjectPool<btCollisionObject>;
    template class ObjectPool<ove::SceneBody>;
    template class ObjectPool<ove::SceneMotionState>;
    
    namespace ove {

SceneDataContext::SceneDataContext(const InitParams& params) :
    _triMeshPool(params.numTriMeshShapes),
    _triMeshShapePool(params.numTriMeshShapes),
    _cylinderShapePool(params.numCylinderShapes),
    _boxShapePool(params.numBoxShapes),
    _compoundShapePool(params.numBoxShapes + params.numCylinderShapes),
    _bodyPool(params.numBodies),
    _sceneBodyPool(params.numBodies),
    _motionStatesPool(params.numBodies)
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
    shapeRefCntInc(triMeshShape);
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
        shapeRefCntInc(shape);
        compShape->addChildShape(localTransform, shape);
        shapeRefCntInc(compShape);
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
        shapeRefCntInc(shape);
        compShape->addChildShape(localTransform, shape);
        shapeRefCntInc(compShape);
    }
    return compShape;
}

btCollisionShape* SceneDataContext::cloneCollisionShape(const btCollisionShape* source)
{
    btCollisionShape* clonedShape = nullptr;
    const btVector3& sourceLocalScaling = source->getLocalScaling();

    switch (source->getShapeType())
    {
    case TRIANGLE_MESH_SHAPE_PROXYTYPE:
        {
            const btBvhTriangleMeshShape* triMeshShape = static_cast<const btBvhTriangleMeshShape*>(source);
            
            //  fixed body hulls do not change - but the reference count changes
            //  so we cast off the const for this special case.
            SceneFixedBodyHull* hull = const_cast<SceneFixedBodyHull*>(
                static_cast<const SceneFixedBodyHull*>(triMeshShape->getMeshInterface())
            );
            hull->incRef();
            clonedShape = allocateTriangleMeshShape(hull, sourceLocalScaling);
            //  ref count already incremented inside allocateTriangleMeshShape
        }
        return clonedShape;
        
    case COMPOUND_SHAPE_PROXYTYPE:
        {
            //  deep clone the parent compound shape
            //  which means duplicating the compound shape
            const btCompoundShape* sourceCompound = static_cast<const btCompoundShape*>(source);
            int numChildShapes = sourceCompound->getNumChildShapes();
            btCompoundShape* compound = _compoundShapePool.construct(true, numChildShapes);

            for (int i = 0; i < numChildShapes; ++i) {
                const btCollisionShape* child = sourceCompound->getChildShape(i);
                btCollisionShape* shape = cloneCollisionShape(child);
                compound->addChildShape(sourceCompound->getChildTransform(i), shape);
            }
            clonedShape = compound;
            shapeRefCntInc(clonedShape);
        }
        break;
    default:
        break;
    }
    
    if (clonedShape) {
        //  non trivial shapes are created from child shapes
        //  so we perform a trivial clone (upping ref counts) on child shapes
        //  while replicating the encompassing parent shape, and thus need to
        //  duplicate attributes
        clonedShape->setLocalScaling(sourceLocalScaling);
        clonedShape->setMargin(source->getMargin());
    }
    else {
        //  cloning here is simply upping the refcount (for trivial shapes)
        clonedShape = const_cast<btCollisionShape*>(source);
        shapeRefCntInc(clonedShape);
    }
    
    return clonedShape;
}

void SceneDataContext::freeShape(btCollisionShape* collisionShape)
{
    shapeRefCntDec(collisionShape);
    if (shapeRefCnt(collisionShape) != 0)
        return;
    
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

SceneBody* SceneDataContext::allocateBody
(
    const SceneBodyInitParams& info,
    gfx::NodeHandle gfxNodeHandle,
    Entity entity
)
{
    SceneMotionState* motionState = nullptr;
    if (gfxNodeHandle) {
        motionState = allocateMotionState(gfxNodeHandle);
    }
    /*
    btRigidBody::btRigidBodyConstructionInfo btInfo(info.mass,
        motionState,
        info.collisionShape);
    */
    auto obj = _bodyPool.construct();
    obj->setCollisionShape(info.collisionShape);
    auto sceneObj = _sceneBodyPool.construct();
    sceneObj->btBody = obj;
    sceneObj->motionState = motionState;
    sceneObj->entity = entity;
    
    return sceneObj;
}

/*
void btRigidBodyCloneConstructionInfo
(
    btRigidBody::btRigidBodyConstructionInfo& info,
    const btRigidBody& source
)
{
    info.m_linearDamping = source.getLinearDamping();
    info.m_angularDamping = source.getAngularDamping();
    info.m_friction = source.getFriction();
    info.m_rollingFriction = source.getRollingFriction();
    info.m_restitution = source.getRestitution();
    info.m_linearSleepingThreshold = source.getLinearSleepingThreshold();
    info.m_angularSleepingThreshold = source.getAngularSleepingThreshold();

    //  TODO - additional values?
}
*/

SceneBody* SceneDataContext::cloneBody
(
    const SceneBody* source,
    gfx::NodeHandle gfxNodeHandle,
    Entity entity
)
{
    /*
    SceneBodyMassProps massProps;
    massProps.fromRigidBody(*source);
    */
    
    //  clone collision shape
    const btCollisionShape* shape = source->btBody->getCollisionShape();
    btCollisionShape* clonedShape = cloneCollisionShape(shape);
    
    CK_ASSERT_RETURN_VALUE(clonedShape != nullptr, nullptr);
    
    SceneMotionState* motionState = nullptr;
    if (gfxNodeHandle) {
        motionState = allocateMotionState(gfxNodeHandle);
    }
    
    /*
    btRigidBody::btRigidBodyConstructionInfo btInfo(massProps.mass,
        motionState,
        clonedShape,
        massProps.inertia);
    
    btRigidBodyCloneConstructionInfo(btInfo, *source);
    */
    auto obj = _bodyPool.construct();
    obj->setCollisionShape(clonedShape);
    
    auto sceneObj = _sceneBodyPool.construct();
    sceneObj->btBody = obj;
    sceneObj->entity = entity;
    sceneObj->motionState = motionState;
    
    return sceneObj;
}

void SceneDataContext::freeBody(SceneBody* body)
{
    freeMotionState(body->motionState);
    freeShape(body->btBody->getCollisionShape());
    _bodyPool.destruct(body->btBody);
    _sceneBodyPool.destruct(body);
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