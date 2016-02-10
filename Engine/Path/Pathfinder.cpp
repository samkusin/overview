//
//  Pathfinder.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/9/16.
//
//

#include "Pathfinder.hpp"

#include "Engine/Tasks/GenerateRecastNavMesh.hpp"
#include "Engine/Physics/Scene.hpp"
#include "Engine/Physics/SceneFixedBodyHull.hpp"
#include "Engine/Physics/SceneMotionState.hpp"

#include <cinek/taskscheduler.hpp>

namespace cinek {
    namespace ove {
    
void pathfinderAddHullToRecastNavMeshInput
(
    const SceneFixedBodyHull& hull,
    const btTransform& transform,
    RecastNavMeshInput& output
)
{

    //  generate Recast ingestible vertices from our source hull data
    //  a transform is supplied in case we're generating a nav-mesh from local
    //  hull data
    
    const float* vertexData = hull.vertexData();
    const int* indexData = hull.indexData();
    
    float* bmin = output.bmin;
    float* bmax = output.bmax;
    
    for (int vidx = 0; vidx < hull.vertexCount(); ++vidx) {
        btVector3 btv(vertexData[0], vertexData[1], vertexData[2]);
        btv = transform.getBasis() * btv;
        btv += transform.getOrigin();
        
        output.vertexData.push_back(btv.getX());
        output.vertexData.push_back(btv.getY());
        output.vertexData.push_back(btv.getZ());
        
        if (btv.getX() < bmin[0]) {
            bmin[0] = btv.getX();
        }
        if (btv.getY() < bmin[1]) {
            bmin[1] = btv.getY();
        }
        if (btv.getZ() < bmin[2]) {
            bmin[2] = btv.getZ();
        }
        if (btv.getX() > bmax[0]) {
            bmax[0] = btv.getX();
        }
        if (btv.getY() > bmax[1]) {
            bmax[1] = btv.getY();
        }
        if (btv.getZ() > bmax[2]) {
            bmax[2] = btv.getZ();
        }
        vertexData += 3;
    }
    
    //  reverse winding order of vertices (recast requires clockwise vertices)
    const int* pindexend = indexData + hull.triangleCount()*3;
    for (const int* pindex = indexData; pindex != pindexend; ++pindex) {
        output.triangleData.push_back(pindex[2]);
        output.triangleData.push_back(pindex[1]);
        output.triangleData.push_back(pindex[0]);
        pindex += 3;
    }
}


class Pathfinder::Impl
{
    TaskScheduler _scheduler;
    TaskId _generateTaskId;
    GenerateCb _generateCb;
    
    void signalGenerateComplete(bool result)
    {
        if (_generateCb) {
            _generateCb(result);
            _generateCb = nullptr;
        }
        _generateTaskId = kNullHandle;
    }
    
    RecastNavMesh _navigationMesh;
    
public:
    Impl() :
        _scheduler(16),
        _generateTaskId(kNullHandle)
    {
    }
    ~Impl()
    {
        _scheduler.cancelAll(this);
    }
    
    void generateFromScene
    (
        const Scene& scene,
        GenerateCb callback
    )
    {
        RecastNavMeshInput meshInput;
     
        if (_generateTaskId) {
            _scheduler.cancel(_generateTaskId);
            signalGenerateComplete(false);
        }
        
        //  generate Recast ingestible vertices from our source hull data
        //  a transform is supplied in case we're generating a nav-mesh from local
        //  hull data
        
        scene.iterateBodies(SceneBody::kIsSection,
            [&meshInput](SceneBody* body, uint32_t ) {
                auto hull = body->getFixedHull();
                if (hull) {
                    meshInput.numVertices += hull->vertexCount();
                    meshInput.numTriangles += hull->triangleCount();
                }
            });
        
        meshInput.vertexData.reserve(meshInput.numVertices*3);
        meshInput.triangleData.reserve(meshInput.numTriangles*3);
        
        scene.iterateBodies(SceneBody::kIsSection,
            [&meshInput](SceneBody* body, uint32_t ) {
                auto hull = body->getFixedHull();
                if (hull) {
                    btTransform worldTransform;
                    body->motionState->getWorldTransform(worldTransform);
                    pathfinderAddHullToRecastNavMeshInput(*hull, worldTransform, meshInput);
                }
            });
  
        auto task = allocate_unique<GenerateRecastNavMesh>(std::move(meshInput));
        task->setCallback([this](Task::State endState, Task& task, void*) {
            if (endState == Task::State::kEnded) {
                auto& thisTask = reinterpret_cast<GenerateRecastNavMesh&>(task);
                _navigationMesh = thisTask.acquireGeneratedMesh();
                signalGenerateComplete(true);
            }
        });
        
        _generateCb = std::move(callback);
        _generateTaskId = _scheduler.schedule(std::move(task), this);
    }

    void update(double dt)
    {
        _scheduler.update((uint32_t)(dt * 1000.0));
    }
};


Pathfinder::Pathfinder() :
    _impl(allocate_unique<Impl>())
{
}

Pathfinder::~Pathfinder()
{
}

void Pathfinder::update(double dt)
{
    _impl->update(dt);
}

void Pathfinder::generateFromScene
(
    const Scene& scene,
    GenerateCb callback
)
{
    _impl->generateFromScene(scene, std::move(callback));
}

    
    } /* namespace ove */
} /* namespace cinek */