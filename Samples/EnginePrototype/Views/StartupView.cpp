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

StartupView::StartupView(ApplicationContext* api) :
    AppViewController(api),
    _templatesLoaded(false),
    _globalsLoaded(false)
{
    
}
    
void StartupView::onViewAdded(ove::ViewStack& stateController)
{
    assetService().loadManifest("global.json",
        [this](std::shared_ptr<ove::AssetManifest> manifest) {
            //entityService().addDefintions("entity", manifest);
            _globalsLoaded = true;
        });
    assetService().loadManifest("entity.json",
        [this](std::shared_ptr<ove::AssetManifest> manifest) {
            entityService().addDefintions("entity", manifest);
            _templatesLoaded = true;
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

void StartupView::simulateView(ove::ViewStack& stateController, CKTimeDelta dt)
{
}

void StartupView::frameUpdateView
(
    ove::ViewStack& stateController,
    CKTimeDelta /* dt */,
    const cinek::uicore::InputState& /* inputState */
)
{
    if (_templatesLoaded && _globalsLoaded) {
        stateController.present("LoadSceneView");
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
