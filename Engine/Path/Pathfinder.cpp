//
//  Pathfinder.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/9/16.
//
//

#include "Pathfinder.hpp"
#include "PathfinderDebug.hpp"
#include "PathfinderListener.hpp"
#include "NavPathQueryPool.hpp"
#include "NavPathQuery.hpp"
#include "RecastMesh.hpp"
#include "NavMesh.hpp"
#include "NavPath.hpp"

#include "Tasks/GenerateNavPath.hpp"

#include "Engine/Contrib/Recast/DetourNavMeshQuery.h"

#include "Engine/Path/Tasks/GenerateRecastMesh.hpp"
#include "Engine/Path/Tasks/GenerateNavMesh.hpp"
#include "Engine/Physics/Scene.hpp"
#include "Engine/Physics/SceneFixedBodyHull.hpp"
#include "Engine/Physics/SceneMotionState.hpp"

#include <cinek/taskscheduler.hpp>
#include <vector>

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
    
    struct EntityTaskInfo
    {
        Entity entity;
        TaskId task;
        PathfinderListener* listener;
        bool operator<(Entity e) const {
            return entity < e;
        }
    };
    
    std::vector<EntityTaskInfo> _tasks;
    
    RecastMeshConfig _navMeshConfig;
    RecastMesh _recastMesh;
    NavMesh _navMesh;
    
    //  query filter used by the main thread owning Pathfinder
    dtQueryFilter _dtDefaultQueryFilter;
    unique_ptr<NavPathQueryPool> _queryPool;
    
    struct Command
    {
        enum Type
        {
            kGeneratePath
        };
        Type type;
        PathfinderListener* listener;
        Entity entity;
        ckm::vector3 startPos;
        ckm::vector3 endPos;
    };
    
    std::vector<Command> _commandQueue;
    
private:
    void registerTask(EntityTaskInfo info)
    {
        auto it = std::lower_bound(_tasks.begin(), _tasks.end(), info.entity);
        CK_ASSERT_RETURN(it == _tasks.end() || it->entity != info.entity);
        
        _tasks.emplace(it, std::move(info));
    }
    
    PathfinderListener* finishTask(Entity entity, TaskId task)
    {
        PathfinderListener* listener = nullptr;
        auto it = std::lower_bound(_tasks.begin(), _tasks.end(), entity);
        
        for (;it != _tasks.end(); ) {
            auto& entry = *it;
            if (entry.entity != entity)
                break;
            
            if (task == entry.task) {
                listener = entry.listener;
                it = _tasks.erase(it);
                break;
            }
            else {
                ++it;
            }
        }
        
        return listener;
    }
    
    bool taskActive(Entity entity)
    {
        auto it = std::lower_bound(_tasks.begin(), _tasks.end(), entity);
        
        for (;it != _tasks.end(); ++it) {
            auto& entry = *it;
            if (entry.entity != entity)
                break;
            
            if (_scheduler.isActive(entry.task))
                return true;
        }
        
        return false;
    }
    
    void signalGenerateComplete(bool result)
    {
        if (_generateCb) {
            _generateCb(result);
            _generateCb = nullptr;
        }
        _generateTaskId = 0;
    }
    
    auto runCommand(Command& cmd) -> std::pair<PathfinderError, bool>
    {
        PathfinderError err = PathfinderError::kNone;
        bool consumeCommand = false;
        
        switch (cmd.type) {
            
        case Command::kGeneratePath:
            if (_queryPool && !taskActive(cmd.entity)) {
                auto query = _queryPool->acquire();
                if (query) {
                    ckm::vector3 extents(ckm::scalar(0.1), ckm::scalar(0.1), ckm::scalar(0.1));
                    auto task = allocate_unique<GenerateNavPath>(std::move(query),
                                    cmd.entity,
                                    cmd.startPos,
                                    cmd.endPos,
                                    extents,
                                    Allocator());
                   
                    task->setCallback([this](Task::State state, Task& t, void*) {
                        auto& genTask = static_cast<GenerateNavPath&>(t);
                        Entity entity = genTask.entity();
                        auto listener = finishTask(entity, genTask.id());
                        if (listener) {
                            if (state == Task::State::kEnded) {
                                auto points = genTask.acquirePoints();
                                NavPath path(std::move(points), genTask.startPos(), genTask.endPos());
                                listener->onPathfinderPathUpdate(entity, std::move(path));
                            }
                            else {
                                listener->onPathfinderError(entity, PathfinderError::kFailure);
                            }
                        }
                    });
                    
                    EntityTaskInfo taskInfo;
                    taskInfo.task = _scheduler.schedule(std::move(task), this);
                    taskInfo.listener = cmd.listener;
                    taskInfo.entity = cmd.entity;
                    if (taskInfo.task) {
                        registerTask(std::move(taskInfo));
                        consumeCommand = true;
                    }
                    else {
                        err = PathfinderError::kOutOfResources;
                    }
                }
                //  no query available, don't execute now
            }
            break;
        
        default:
            CK_ASSERT(false);
            break;
            
        }
        
        return std::make_pair(err, consumeCommand);
    }

    
public:
    Impl() :
        _scheduler(16),
        _generateTaskId(0)
    {
        //  TODO - magic numbers! consolidate into an InitParams
        _tasks.reserve(32);
        _commandQueue.reserve(256);
    }
    
    ~Impl()
    {
        _scheduler.cancelAll(this);
    }
    
    //  cancels commands by listener
    void cancelByListener(PathfinderListener* listener)
    {
        if (!listener)
            return;
        
        //  clear pending and active commands
        for (auto it = _commandQueue.begin(); it != _commandQueue.end(); ) {
            if (it->listener == listener) {
                it = _commandQueue.erase(it);
            }
            else {
                ++it;
            }
        }
        
        for (auto it = _tasks.begin(); it != _tasks.end(); ) {
            auto& taskInfo = *it;
            if (taskInfo.listener == listener) {
                _scheduler.cancel(taskInfo.task);
                it = _tasks.erase(it);
            }
            else {
                ++it;
            }
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////
    //  Generates nav mesh from the input scene hull.
    //  This is done mainly for in-game editing.   Production-ready missions
    //  would load generated data.
    //
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
                        
                        NavPathQueryPool::InitParams initParams;
                        initParams.navMesh = &_navMesh;
                        initParams.numQueries = 32;
                        _queryPool = allocate_unique<NavPathQueryPool>(initParams);
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

    ////////////////////////////////////////////////////////////////////////////
    //  instantiate a path query task
    //
    void generatePath
    (
        PathfinderListener* listener,
        Entity entity,
        ckm::vector3 startPos,
        ckm::vector3 endPos
    )
    {
        Command cmd = {
            Command::kGeneratePath,
            listener,
            entity,
            startPos,
            endPos
        };
        _commandQueue.emplace_back(cmd);
    }
    
    ////////////////////////////////////////////////////////////////////////////
    //  obtain a query object for use
    //
    NavPathQueryPtr acquireQuery()
    {
        return _queryPool->acquire();
    }

    ////////////////////////////////////////////////////////////////////////////
    //  Updates main scheduler and synchronized with any path-query threads
    //
    void simulate(CKTimeDelta dt)
    {
        auto cmdIt = _commandQueue.begin();
        auto cmdItEnd = _commandQueue.end();
        
        //  process commands
        //  not all commmands are processed this frame based on available
        //  resources like available queries, tasks, etc.
        bool consumeCommand = true;
        while (cmdIt != cmdItEnd) {
            auto& cmd = *cmdIt;
            auto result = runCommand(cmd);

            if (result.first != PathfinderError::kNone && cmd.listener) {
                cmd.listener->onPathfinderError(cmd.entity, result.first);
            }

            consumeCommand = result.second;
            if (consumeCommand) {
                ++cmdIt;
            }
            else {
                break;
            }
        }
        // remove used commands from queue
        _commandQueue.erase(_commandQueue.begin(), cmdIt);
    
        //  update tasks
        _scheduler.update((uint32_t)(dt * 1000.0));
    }
    
    ////////////////////////////////////////////////////////////////////////////
    //  Handles debugging pathfinding requests
    //
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

void Pathfinder::cancelByListener(PathfinderListener* listener)
{
    _impl->cancelByListener(listener);
}

void Pathfinder::simulate(CKTimeDelta dt)
{
    _impl->simulate(dt);
}

void Pathfinder::generateFromScene
(
    const Scene& scene,
    GenerateCb callback
)
{
    _impl->generateFromScene(scene, std::move(callback));
}

void Pathfinder::simulateDebug(PathfinderDebug& debugger)
{
    _impl->updateDebug(debugger);
}

NavPathQueryPtr Pathfinder::acquireQuery()
{
    return _impl->acquireQuery();
}

void Pathfinder::generatePath
(
    PathfinderListener* listener,
    Entity entity,
    ckm::vector3 startPos,
    ckm::vector3 endPos
)
{
    _impl->generatePath(listener, entity, startPos, endPos);
}
    
    } /* namespace ove */
} /* namespace cinek */