//
//  SceneFixedBodyFactory.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/3/15.
//
//

#include "SceneDataContext.hpp"
#include "SceneObject.hpp"

namespace cinek {
    namespace ove {

SceneDataContext::SceneDataContext(const InitParams& params) :
    _triMeshPool(params.numTriMeshShapes),
    _triMeshShapePool(params.numTriMeshShapes),
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

void SceneDataContext::freeTriangleMeshShape(btBvhTriangleMeshShape* shape)
{
    SceneFixedBodyHull* hull = static_cast<SceneFixedBodyHull*>(shape->getMeshInterface());
    releaseFixedBodyHull(hull);
    _triMeshShapePool.destruct(shape);
}



btRigidBody* SceneDataContext::allocateBody
(
    const SceneBodyInitParams& info,
    gfx::NodeHandle gfxNodeHandle
)
{
    SceneMotionState* motionState = _motionStatesPool.construct(gfxNodeHandle);
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
        _motionStatesPool.destruct(state);
    }
    _rigidBodyPool.destruct(body);
}

    }
}