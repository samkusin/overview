//
//  StartupView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

#include "StartupView.hpp"

#include "Engine/ViewAPI.hpp"

namespace cinek {
    namespace ove {

StartupView::StartupView(ViewAPI& api) :
    _viewAPI(&api),
    _state(kStart)
{
    
}
    
void StartupView::onViewLoad()
{
}

void StartupView::onViewUnload()
{
}

void StartupView::onViewAdded()
{
    _state = kLoad;
    _viewAPI->entityService().loadDefinitions("entity.json",
        [this](const EntityLoadDefinitionsResponse& resp) {
            _state = kEnd;
            printf("Loaded %s\n", resp.name);
        });
}

void StartupView::onViewRemoved()
{
}

void StartupView::onViewForeground()
{
}

void StartupView::onViewBackground()
{
}

void StartupView::simulateView(double time, double dt)
{
}

void StartupView::layoutView()
{
}

void StartupView::frameUpdateView(double dt)
{
    if (_state == kEnd) {
        _viewAPI->viewService(*this).present("GameView");
    }
}

const char* StartupView::viewId() const
{
    return "StartupView";
}

    } /* namespace ove */
} /* namespace cinek */
