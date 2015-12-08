//
//  GameView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

#include "GameView.hpp"

#include "Engine/ViewAPI.hpp"
#include "Engine/Services/SceneService.hpp"

namespace cinek {
    namespace ove {

GameView::GameView(ViewAPI& api) :
    _viewAPI(&api)
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
