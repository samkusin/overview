//
//  GameView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

#include "GameView.hpp"

namespace cinek {

GameView::GameView(const ApplicationContext& api) :
    AppViewController(api)
{
    
}

/*
void StartupView::loadScene()
{
    _viewAPI->sceneService().loadScene("scenes/ship_bridge.json",
        [this](const SceneLoadResponse& resp) {
            printf("Loaded %s\n", resp.name);
            endView();
        });
}

void StartupView::endView()
{
    _viewAPI->viewService(*this).present("GameView");
}
*/

    
void GameView::onViewAdded(ove::ViewStack& stateController)
{
    assetService().loadManifest("scenes/cityblock.json",
        [this](std::shared_ptr<ove::AssetManifest> manifest) {
            sceneService().initializeScene(manifest);
            entityService().createEntity(0, "entity", "test_bot");
        });
}

void GameView::onViewRemoved(ove::ViewStack& stateController)
{
}

void GameView::onViewForeground(ove::ViewStack& stateController)
{
}

void GameView::onViewBackground(ove::ViewStack& stateController)
{
}

void GameView::onViewStartFrame(ove::ViewStack& stateController)
{
}

void GameView::simulateView(ove::ViewStack& stateController, double dt)
{
}

void GameView::frameUpdateView(ove::ViewStack& stateController, double dt)
{
}

void GameView::onViewEndFrame(ove::ViewStack& stateController)
{
}

const char* GameView::viewId() const
{
    return "GameView";
}

} /* namespace cinek */
