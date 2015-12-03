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

StartupView::StartupView(GameViewAPI& api) :
    _viewAPI(&api)
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
    _viewAPI->entityService().loadDefinitions("entity.json",
        [](const EntityLoadDefinitionsResponse& resp) {
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
}

const char* StartupView::viewId() const
{
    return "StartupView";
}

    } /* namespace ove */
} /* namespace cinek */
