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
    _triMeshShapes()
{
    _triMeshShapes.reserve(params.numTriMeshShapes);
}

SceneFixedBodyHull* SceneDataContext::allocateFixedBodyHull
(
    const SceneFixedBodyHull::VertexIndexCount& counts
)
{
    return _triMeshPool.construct(counts);
}

void SceneDataContext::freeFixedBodyHull(SceneFixedBodyHull* hull)
{
    _triMeshPool.destruct(hull);
}

SceneTriangleMeshShape* SceneDataContext::allocateTriangleMeshShape
(
    SceneFixedBodyHull* hull,
    std::string name
)
{
    auto it = std::lower_bound(_triMeshShapes.begin(), _triMeshShapes.end(), name,
        [](const SceneTriangleMeshShape* obj, const std::string& name) -> bool {
            return obj->name() < name;
        });
    if (it != _triMeshShapes.end() && (*it)->name() == name)
        return nullptr;
    
    it = _triMeshShapes.emplace(it, _triMeshShapePool.construct(hull, std::move(name)));
    (*it)->incRef();
    
    return *it;
}

SceneTriangleMeshShape* SceneDataContext::triangleMeshShapeAcquire
(
    const std::string& name
)
{
    auto it = std::lower_bound(_triMeshShapes.begin(), _triMeshShapes.end(), name,
        [](const SceneTriangleMeshShape* obj, const std::string& name) -> bool {
            return obj->name() < name;
        });
    if (it == _triMeshShapes.end() || (*it)->name() != name)
        return nullptr;
    
    (*it)->incRef();
    return *it;
}

void SceneDataContext::tringleMeshShapeRelease(SceneTriangleMeshShape* shape)
{
    auto it = std::lower_bound(_triMeshShapes.begin(), _triMeshShapes.end(), shape->name(),
        [](const SceneTriangleMeshShape* obj, const std::string& name) -> bool {
            return obj->name() < name;
        });
    if (it == _triMeshShapes.end() || (*it)->name() != shape->name())
        return;
    
    auto obj = *it;
    obj->decRef();
    if (obj->refCnt() <= 0) {
        _triMeshShapes.erase(it);
        auto triMeshHull = static_cast<SceneFixedBodyHull*>(obj->impl()->getMeshInterface());
        if (triMeshHull) {
            _triMeshPool.destruct(triMeshHull);
        }
        _triMeshShapePool.destruct(obj);
    }
}

SceneBody* SceneDataContext::allocateBody
(
    const btRigidBody::btRigidBodyConstructionInfo& info,
    Entity entity
)
{
    auto obj = _rigidBodyPool.construct(info, entity);
    obj->incRef();
    return obj;
}

void SceneDataContext::releaseBody(SceneBody* body)
{
    body->decRef();
    if (body->refCnt() <= 0) {
        _rigidBodyPool.destruct(body);
    }
}

    }
}