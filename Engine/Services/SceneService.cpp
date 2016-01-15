//
//  SceneService.inl
//  EnginePrototype
//
//  Created by Samir Sinha on 12/02/15.
//
//

#include "SceneService.hpp"
#include "RenderService.hpp"
#include "Engine/Scenes/Scene.hpp"
#include "Engine/Scenes/SceneDebugDrawer.hpp"
#include "Engine/Render/RenderGraph.hpp"
#include "Engine/SceneJsonLoader.hpp"
#include "Engine/AssetManifest.hpp"
#include "Engine/Debug.hpp"

#include <ckmsg/client.hpp>
#include <cmath>

namespace cinek {
    namespace ove {

SceneService::SceneService
(
    const SceneServiceContext& context
) :
    _context(context)
{
}


void SceneService::initialize(std::shared_ptr<AssetManifest> manifest)
{
    //  use shared pointer in case we need to persist the manifest (like we do for entity
    //  manifests.)

    SceneJsonLoader loader(*_context.sceneData,
                           *_context.gfxContext,
                           *_context.entityDb);
            
    loader(*_context.scene, *_context.renderGraph, manifest->root());
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
    btRigidBody* body = _context.scene->findBody(entity);
    CK_ASSERT_RETURN(body);
    CK_ASSERT(body->isStaticOrKinematicObject());
    
    if (up.isZero()) {
        up.setValue(0,1,0);
    }
    
    btVector3 forward(0,0,1);
    
    //  determine forward vector based on our world up direction
    btScalar dirdot = std::abs(forward.dot(up));
    if (dirdot > btScalar(0.90)) {
        if (up.y() >=btScalar(0)) {
            forward = btVector3(0,-1,0);
        }
        else {
            forward = btVector3(0,1,0);
        }
    }

    btVector3 side = btCross(forward, up);
    side.normalize();
    
    forward = btCross(up, side);
    forward.normalize();
    
    btTransform transform;
    transform.getBasis().setValue(side.x(), side.y(), side.z(),
                                  up.x(), up.y(), up.z(),
                                  -forward.x(), -forward.y(), -forward.z());
    transform.setOrigin(pos);
    
    body->setCenterOfMassTransform(transform);
    if (body->getMotionState()) {
        body->getMotionState()->setWorldTransform(transform);
    }
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
