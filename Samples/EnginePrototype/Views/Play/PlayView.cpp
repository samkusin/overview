//
//  PlayView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/18/16.
//
//

#include "PlayView.hpp"
#include "PlayMain.hpp"

namespace cinek {

PlayView::PlayView(GameViewContext* gameContext) :
    _gc(gameContext)
{
    _mainState = std::allocate_shared<PlayMain>(std_allocator<PlayMain>(), gameContext);
}
    
void PlayView::onViewAdded(ove::ViewStack& stateController)
{
    _gc->setMode(GameViewContext::Mode::kPlay);
    

    
    _viewStack.setFactory(
        [this](const std::string& viewName, ove::ViewController* )
            -> std::shared_ptr<ove::ViewController> {
            //  Startup View initialzes common data for the application
            //  Game View encompasses the whole game simulation
            //  Ship View encompasses the in-game ship mode
            //  Ship Bridge View encompasses the in-game ship bridge mode
            if (viewName == "PlayMain") {
                return _mainState;
            }
                
            return nullptr;
        });
    
    _viewStack.present("PlayMain");
}

void PlayView::onViewRemoved(ove::ViewStack& stateController)
{
    _viewStack.pop();

    _gc->setMode(GameViewContext::Mode::kNone);
}

void PlayView::onViewStartFrame(ove::ViewStack& stateController)
{
    _viewStack.startFrame();
}

void PlayView::simulateView(ove::ViewStack& stateController, double dt)
{
    _viewStack.simulate(dt);
}

void PlayView::frameUpdateView
(
    ove::ViewStack& stateController,
    double dt,
    const cinek::uicore::InputState& inputState
)
{

    _viewStack.frameUpdate(dt, inputState);
}

void PlayView::onViewEndFrame(ove::ViewStack& stateController)
{
    _viewStack.endFrame();
    
    if (_gc->getMode() == GameViewContext::Mode::kEditor) {
        stateController.present("EditorView");
    }
}
    
const char* PlayView::viewId() const
{
    return "PlayView";
}

}