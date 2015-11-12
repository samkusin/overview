//
//  GameView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

#include "GameView.hpp"
#include "AppContext.hpp"

#include "Engine/Services/EntityService.hpp"

namespace cinek {
    namespace ove {

GameView::GameView(AppContext* appContext) :
    _appContext(appContext)
{
    
}
    
void GameView::onViewLoad()
{
}

void GameView::onViewUnload()
{
}

void GameView::onViewAdded()
{
    EntityService entityService(_appContext->entityUtility);
    
    entityService.addDefinitions("main", "entity.json");
}

void GameView::onViewRemoved()
{
}

void GameView::onViewForeground()
{
}

void GameView::onViewBackground()
{
}

void GameView::simulateView(double time, double dt)
{
}

void GameView::layoutView()
{
}

void GameView::frameUpdateView(double dt)
{
}

const char* GameView::viewId() const
{
    return "GameView";
}

    } /* namespace ove */
} /* namespace cinek */
