//
//  AppViewController.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/22/15.
//
//

#include "AppViewController.hpp"

#include <cinek/taskscheduler.hpp>

namespace cinek {

AppViewController::AppViewController
(
    ApplicationContext* context
) :
    _appContext(context),
    _assetService({ context->taskScheduler, context->resourceFactory }),
    _entityService( context->entityDatabase ),
    _sceneService({ context->gfxContext, context->scene, context->sceneData,
                    context->sceneDebugDrawer,
                    context->entityDatabase,
                    context->renderGraph,
                    context->taskScheduler }),
    _renderService(context->gfxContext, context->renderContext),
    _pathfinderService( { context->scene, context->pathfinder } )
{
}

AppViewController::~AppViewController()
{
    _appContext->taskScheduler->cancelAll(this);
}

}

