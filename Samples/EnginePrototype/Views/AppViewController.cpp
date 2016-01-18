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
    const ApplicationContext& context
) :
    _appContext(&context),
    _assetService({ context.taskScheduler, context.resourceFactory }),
    _entityService( context.entityDatabase, context.msgClientSender ),
    _sceneService({ context.gfxContext, context.scene, context.sceneData,
                    context.sceneDebugDrawer,
                    context.msgClientSender, context.entityDatabase,
                    context.renderGraph }),
    _renderService(context.gfxContext, context.renderContext)
{
}

AppViewController::~AppViewController()
{
    _appContext->taskScheduler->cancelAll(this);
}

}

