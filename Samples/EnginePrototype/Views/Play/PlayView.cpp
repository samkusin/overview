//
//  PlayView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/18/16.
//
//

#include "PlayView.hpp"
#include "PlayMain.hpp"

#include "Engine/Physics/Scene.hpp"
#include "Engine/Controller/NavSystem.hpp"

namespace cinek {

PlayView::PlayView(GameViewContext* gameContext) :
    GameState(gameContext)
{
    _mainState = std::allocate_shared<PlayMain>(std_allocator<PlayMain>(), gameContext);
}
    
void PlayView::onViewAdded(ove::ViewStack& stateController)
{
    game().setGameMode(GameMode::kPlay);
    
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
    
    
    
    scene().activate();
    navSystem().activate();
}

void PlayView::onViewRemoved(ove::ViewStack& stateController)
{
    _viewStack.pop();

    game().setGameMode(GameMode::kNone);
}

void PlayView::onViewStartFrame(ove::ViewStack& stateController)
{
    _viewStack.startFrame();
}

void PlayView::simulateView(ove::ViewStack& stateController, CKTimeDelta dt)
{
    /*
    auto entity = game().getFocusedGameEntity();
    if (entity) {
        auto body = scene().findBody(entity);
        if (body) {
            ckm::vector3 avel(0,1,0);
            
            ckm::scale(avel, avel, dt);
        
            body->setVelocity(ckm::vector3::kZero, avel);
        }
    }
    */

    _viewStack.simulate(dt);
}

void PlayView::frameUpdateView
(
    ove::ViewStack& stateController,
    CKTimeDelta dt,
    const cinek::input::InputState& inputState
)
{

    _viewStack.frameUpdate(dt, inputState);
}

void PlayView::onViewEndFrame(ove::ViewStack& stateController)
{
    _viewStack.endFrame();
    
    if (game().getGameMode() == GameMode::kEditor) {
        stateController.present("EditorView");
    }
}
    
const char* PlayView::viewId() const
{
    return "PlayView";
}

}