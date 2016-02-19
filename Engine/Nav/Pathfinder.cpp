//
//  Pathfinder.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/9/16.
//
//

#include "Pathfinder.hpp"
#include "PathfinderDebug.hpp"
#include "RecastMesh.hpp"
#include "NavMesh.hpp"

#include "Engine/Contrib/Recast/DetourNavMeshQuery.h"

#include "Engine/Tasks/GenerateRecastMesh.hpp"
#include "Engine/Tasks/GenerateNavMesh.hpp"
#include "Engine/Physics/Scene.hpp"
#include "Engine/Physics/SceneFixedBodyHull.hpp"
#include "Engine/Physics/SceneMotionState.hpp"

#include <cinek/taskscheduler.hpp>

namespace cinek {
    namespace ove {
    
void pathfinderAddHullToRecastMeshInput
(
    const SceneFixedBodyHull& hull,
    const btTransform& transform,
    RecastMeshInput& output
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
        btVector3 btv0(vertexData[0], vertexData[1], vertexData[2]);
        btVector3 btv = transform.getBasis() * btv0;
        btv += transform.getOrigin();
        
        output.vertexData.push_back(btv.getX());
        output.vertexData.push_back(btv.getY());
        output.vertexData.push_back(btv.getZ());
        
        /*
        printf("(%5.2f,%5.2f,%5.2f) -> (%5.2f,%5.2f,%5.2f)\n",
               btv0.getX(), btv0.getY(), btv0.getZ(),
               btv.getX(), btv.getY(), btv.getZ());
        */
        
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
    for (const int* pindex = indexData; pindex != pindexend; pindex += 3) {
        output.triangleData.push_back(pindex[2]);
        output.triangleData.push_back(pindex[1]);
        output.triangleData.push_back(pindex[0]);
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


    RecastMeshConfig _navMeshConfig;
    RecastMesh _recastMesh;
    NavMesh _navMesh;
    
    //  query filter used by the main thread owning Pathfinder
    dtQueryFilter _mainQueryFilter;
    detour_nav_query_unique_ptr _mainQuery;
    
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
       RecastMeshInput meshInput;
     
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
                    pathfinderAddHullToRecastMeshInput(*hull, worldTransform, meshInput);
                }
            });
        
        _navMeshConfig.cellSize = 0.20f;
        _navMeshConfig.cellHeight = 0.025f;
        _navMeshConfig.walkableClimb = 0.5f;
        _navMeshConfig.walkableRadius = 0.1f;
        _navMeshConfig.walkableHeight = 2.0f;
  
        auto task = allocate_unique<GenerateRecastMesh>(_navMeshConfig, std::move(meshInput));
        task->setCallback([this](Task::State endState, Task& task, void*) {
            if (endState == Task::State::kEnded) {
                auto& thisTask = reinterpret_cast<GenerateRecastMesh&>(task);
                _recastMesh = thisTask.acquireGeneratedMesh(GenerateRecastMesh::kOutputMesh);
                
                auto nexttask = allocate_unique<GenerateNavMesh>(
                    _navMeshConfig,
                    _recastMesh.acquirePolyMesh(),
                    _recastMesh.acquireDetailMesh());
                
                nexttask->setCallback([this](Task::State endState, Task& task, void*) {
                    if (endState == Task::State::kEnded) {
                        auto& thisTask = reinterpret_cast<GenerateNavMesh&>(task);
                        _navMesh = thisTask.acquireGeneratedMesh();
                        _mainQuery = _navMesh.createQuery(2048);
                    }
                    signalGenerateComplete(endState == Task::State::kEnded);
                });
                
                task.setNextTask(std::move(nexttask));
            }
            else {
                signalGenerateComplete(false);
            }
        });
        
        _generateCb = std::move(callback);
        _generateTaskId = _scheduler.schedule(std::move(task), this);
    }


    bool isLocationWalkable
    (
        ckm::vector3f pos,
        ckm::vector3f extents
    )
    {
        if (!_mainQuery)
            return false;
        
        _mainQueryFilter.setIncludeFlags(kNavMeshPoly_Walkable);
        _mainQueryFilter.setExcludeFlags(0);
        
        dtPolyRef resultRef = 0;
        dtStatus status = _mainQuery->findNearestPoly(pos, extents, &_mainQueryFilter, &resultRef, nullptr);
        if (dtStatusFailed(status))
            return false;
        
        return resultRef != 0;
    }

    void update(double dt)
    {
        _scheduler.update((uint32_t)(dt * 1000.0));
    }
    
    void updateDebug(PathfinderDebug& debugger)
    {
        _navMesh.debugDraw(debugger);
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

void Pathfinder::updateDebug(PathfinderDebug& debugger)
{
    _impl->updateDebug(debugger);
}

bool Pathfinder::isLocationWalkable
(
    ckm::vector3f pos,
    ckm::vector3f extents
)
{
    return _impl->isLocationWalkable(pos, extents);
}

    
    } /* namespace ove */
} /* namespace cinek */