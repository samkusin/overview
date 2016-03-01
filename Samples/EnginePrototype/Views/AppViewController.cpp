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
    _pathfinder( context->pathfinder ),
    _pathfinderDebug( context->pathfinderDebug ),
    _uiService( context->uiContext )
{
}

AppViewController::~AppViewController()
{
    _appContext->taskScheduler->cancelAll(this);
}

}

