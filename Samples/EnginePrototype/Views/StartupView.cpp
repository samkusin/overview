//
//  StartupView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

#include "StartupView.hpp"
#include "Engine/ViewStack.hpp"

namespace cinek {

StartupView::StartupView(const ApplicationContext& api) :
    AppViewController(api),
    _loadCompleted(false)
{
    
}
    
void StartupView::onViewAdded(ove::ViewStack& stateController)
{
    assetService().loadManifest("entity.json",
        [this](std::shared_ptr<ove::AssetManifest> manifest) {
            entityService().addDefintions("entity", manifest);
            _loadCompleted = true;
        });
}

void StartupView::onViewRemoved(ove::ViewStack& stateController)
{
}

void StartupView::onViewForeground(ove::ViewStack& stateController)
{
}

void StartupView::onViewBackground(ove::ViewStack& stateController)
{
}

void StartupView::onViewStartFrame(ove::ViewStack& stateController)
{
}

void StartupView::simulateView(ove::ViewStack& stateController, double dt)
{
}

void StartupView::frameUpdateView
(
    ove::ViewStack& stateController,
    double /* dt */,
    const cinek::uicore::InputState& /* inputState */
)
{
    if (_loadCompleted) {
        stateController.present("GameView");
    }
}

void StartupView::onViewEndFrame(ove::ViewStack& stateController)
{
}

const char* StartupView::viewId() const
{
    return "StartupView";
}

} /* namespace cinek */
