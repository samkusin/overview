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

#include "Engine/Nav/Tasks/GenerateRecastMesh.hpp"
#include "Engine/Nav/Tasks/GenerateNavMesh.hpp"
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
    
    std::vector<std::pair<Entity, TaskId>> _tasks;
    
    RecastMeshConfig _navMeshConfig;
    RecastMesh _recastMesh;
    NavMesh _navMesh;
    
    //  query filter used by the main thread owning Pathfinder
    unique_ptr<NavPathQueryPool> _queryPool;
    
    //  report listeners
    PathfinderListener* _listener;
    
    struct Command
    {
        enum Type
        {
            kGeneratePath
        };
        Type type;
        Entity entity;
        ckm::vector3f startPos;
        ckm::vector3f endPos;
    };
    
    std::vector<Command> _commandQueue;
    
private:
    void signalGenerateComplete(bool result)
    {
        if (_generateCb) {
            _generateCb(result);
            _generateCb = nullptr;
        }
        _generateTaskId = kNullHandle;
    }
    
    void completeGeneratePath(Task::State state, GenerateNavPath& task)
    {
        if (state == Task::State::kEnded) {
            auto points = task.acquirePoints();
            NavPath path(std::move(points));
            signalListeners(&PathfinderListener::onPathfinderPathUpdate, task.entity(), std::move(path));
        }
        else if (state == Task::State::kFailed) {
            signalListeners(&PathfinderListener::onPathfinderError, task.entity(), PathfinderError::kTaskFailure);
        }
    }
    
    void registerTask(Entity entity, TaskId task)
    {
        auto it = std::lower_bound(_tasks.begin(), _tasks.end(), entity,
            [](const decltype(_tasks)::value_type& data, Entity e) -> bool {
                return data.first < e;
            });
        
        CK_ASSERT_RETURN(it == _tasks.end() || (*it).first != entity);
        
        _tasks.emplace(it, entity, task);
    }
    
    void cleanupTask(Entity entity, TaskId task, bool kill)
    {
        auto it = std::lower_bound(_tasks.begin(), _tasks.end(), entity,
            [](const decltype(_tasks)::value_type& data, Entity e) -> bool {
                return data.first < e;
            });
        
        for (;it != _tasks.end(); ) {
            auto& entry = *it;
            if (entry.first != entity)
                break;
            
            if (!task || task == entry.second) {
                if (kill) {
                    _scheduler.cancel(entry.second);
                }
                it = _tasks.erase(it);
            }
            else {
                ++it;
            }
        }
    }
    
    bool taskActive(Entity entity)
    {
        auto it = std::lower_bound(_tasks.begin(), _tasks.end(), entity,
            [](const decltype(_tasks)::value_type& data, Entity e) -> bool {
                return data.first < e;
            });
        
        for (;it != _tasks.end(); ++it) {
            auto& entry = *it;
            if (entry.first != entity)
                break;
            
            if (_scheduler.isActive(entry.second))
                return true;
        }
        
        return false;
    }
    
    template<typename Fn>
    void signalListeners(Fn fn)
    {
        if (_listener) {
            (_listener->*fn)();
        }
    }
    
    template<typename Fn, typename... P>
    void signalListeners(Fn fn, P&&... params)
    {
        if (_listener) {
            (_listener->*fn)(std::forward<P>(params)...);
        }
    }
    
public:
    Impl() :
        _scheduler(16),
        _generateTaskId(kNullHandle),
        _listener(nullptr)
    {
        //  TODO - magic numbers! consolidate into an InitParams
        _tasks.reserve(32);
        _commandQueue.reserve(256);
    }
    
    ~Impl()
    {
        _scheduler.cancelAll(this);
    }
    
    ////////////////////////////////////////////////////////////////////////////
    //  listener management
    //
    void setListener(PathfinderListener* listener)
    {
        _listener = listener;
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
    //  Detect whether a position on the mesh is walkable.  This isn't entirely
    //  accurate but works for now.
    //
    bool isLocationWalkable
    (
        ckm::vector3f pos,
        ckm::vector3f extents
    )
    {
        if (!_queryPool)
            return false;
    
        auto query = _queryPool->acquire();
        if (!query)
            return false;
        query->setupFilters(kNavMeshPoly_Walkable);
        return query->isWalkable(pos, extents);
    }
    
    ////////////////////////////////////////////////////////////////////////////
    //  instantiate a path query task
    //
    void generatePath
    (
        Entity entity,
        ckm::vector3f startPos,
        ckm::vector3f endPos
    )
    {
        Command cmd = {
            Command::kGeneratePath,
            entity,
            startPos,
            endPos
        };
        _commandQueue.emplace_back(cmd);
    }
    

    ////////////////////////////////////////////////////////////////////////////
    //  Updates main scheduler and synchronized with any path-query threads
    //
    void update(double dt)
    {
        auto cmdIt = _commandQueue.begin();
        auto cmdItEnd = _commandQueue.end();
        
        //  process commands
        //  not all commmands are processed this frame based on available
        //  resources like available queries, tasks, etc.
        bool consumeCommand = true;
        while (consumeCommand && cmdIt != cmdItEnd) {
            auto& cmd = *cmdIt;
            PathfinderError err = PathfinderError::kNone;
            
            consumeCommand = false;
    
            switch (cmd.type) {
            
            case Command::kGeneratePath:
                if (_queryPool) {
                    if (!taskActive(cmd.entity)) {
                        auto query = _queryPool->acquire();
                        if (query) {
                            auto task = allocate_unique<GenerateNavPath>(std::move(query),
                                            cmd.entity,
                                            cmd.startPos,
                                            cmd.endPos);
                           
                            task->setCallback([this](Task::State state, Task& t, void*) {
                                auto& genTask = static_cast<GenerateNavPath&>(t);
                                completeGeneratePath(state, genTask);
                                cleanupTask(genTask.entity(), genTask.id(), false);
                            });
                            
                            auto taskId = _scheduler.schedule(std::move(task), this);
                            if (taskId) {
                                registerTask(cmd.entity, taskId);
                                consumeCommand = true;
                            }
                            else {
                                err = PathfinderError::kOutOfResources;
                            }
                        }
                        //  no query available, don't execute now
                    }
                }
                else {
                    err = PathfinderError::kFailure;
                }
                break;
            
            default:
                CK_ASSERT(false);
                break;
            }
            
            if (err != PathfinderError::kNone) {
                signalListeners(&PathfinderListener::onPathfinderError, cmd.entity, err);
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

void Pathfinder::setListener(PathfinderListener* listener)
{
    _impl->setListener(listener);
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

void Pathfinder::generatePath
(
    Entity entity,
    ckm::vector3f startPos,
    ckm::vector3f endPos
)
{
    _impl->generatePath(entity, startPos, endPos);
}
    
    } /* namespace ove */
} /* namespace cinek */