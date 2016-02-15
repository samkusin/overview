//
//  SceneService.inl
//  EnginePrototype
//
//  Created by Samir Sinha on 12/02/15.
//
//

#include "SceneService.hpp"
#include "RenderService.hpp"
#include "Engine/Physics/Scene.hpp"

#include "Engine/Physics/SceneDebugDrawer.hpp"
#include "Engine/Render/RenderGraph.hpp"
#include "Engine/SceneJsonLoader.hpp"
#include "Engine/AssetManifest.hpp"
#include "Engine/Debug.hpp"

#include "Engine/Tasks/InitializeScene.hpp"

#include <cinek/taskscheduler.hpp>
#include <ckmsg/client.hpp>


namespace cinek {
    namespace ove {

SceneService::SceneService
(
    const SceneServiceContext& context
) :
    _context(context)
{
}

SceneService::~SceneService()
{
    _context.taskScheduler->cancelAll(this);
}


void SceneService::load
(
    std::shared_ptr<AssetManifest> manifest,
    std::function<void(bool)> cb
)
{
    SceneJsonLoader loader(
        _context.sceneData,
        _context.gfxContext,
        _context.renderGraph,
        _context.entityDb
    );
    
    //  InitializeScene and InitializePaths tasks
    //  together
    
   _context.taskScheduler->schedule(allocate_unique<InitializeScene>(
        std::move(manifest),
        loader,
        [cb](Task::State endstate, Task& thisTask, void* context) {
            SceneService* service = reinterpret_cast<SceneService*>(context);
            if (endstate == Task::State::kEnded) {
                auto bodies = reinterpret_cast<InitializeScene&>(thisTask).acquireBodyList();
                
                for (auto& body : bodies) {
                    service->_context.scene->attachBody(body, body->categoryMask);
                }
                
                cb(true);
            }
            else {
                cb(false);
            }
        }),
        this);
}

void SceneService::disableSimulation()
{
    return _context.scene->deactivateSimulation();
}

void SceneService::enableSimulation()
{
    return _context.scene->activateSimulation();
}

bool SceneService::isSimulationRunning() const
{
    return _context.scene->isActive();
}


void SceneService::setEntityPosition
(
    Entity entity,
    const btVector3& pos,
    btVector3 up
)
{
    SceneBody* body = _context.scene->findBody(entity);
    CK_ASSERT_RETURN(body);
    
    body->setPosition(pos, up);
}

void SceneService::renderDebugAddRayTestHit
(
    const SceneRayTestResult& rayTestResult,
    const btVector3& origin,
    btScalar radius,
    bool drawRay)
{
    if (!rayTestResult)
        return;
        
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(rayTestResult.position);
        
    _context.debugDrawer->drawSphere(radius, transform, btVector3(0,0.5,1.0f));
    _context.debugDrawer->drawLine(rayTestResult.position,
        rayTestResult.position + rayTestResult.normal,
        btVector3(0,0.5,1.0f));
    
    if (drawRay) {
        _context.debugDrawer->drawLine(origin,
            rayTestResult.position,
            btVector3(0,0,1.0f));
    }
}

void SceneService::renderDebugStart
(
    RenderService &renderService,
    const gfx::Camera& camera
)
{
    const RenderContext& renderContext = renderService.renderContext();
    _context.debugDrawer->setup(*renderContext.programs, *renderContext.uniforms, camera);
}

void SceneService::renderDebugAddLine
(
    const btVector3& p0,
    const btVector3& p1,
    const gfx::Color3& color
)
{
    _context.debugDrawer->drawLine(p0, p1, btVector3(color.x,color.y,color.z));
}

void SceneService::renderDebugEnd()
{
    _context.scene->debugRender();
}

gfx::NodeHandle SceneService::getGfxRootNode() const
{
    return _context.renderGraph->root();
}

SceneRayTestResult SceneService::rayTestClosest
(
    const btVector3& origin,
    const btVector3& dir,
    btScalar dist
)
const
{
    return _context.scene->rayTestClosest(origin, dir, dist);
}

    } /* namespace ove */
} /* namespace cinek */
