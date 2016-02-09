//
//  EditorMain.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/15/16.
//
//

#include "EditorMain.hpp"

#include "Engine/ViewStack.hpp"

namespace cinek {


EditorMain::EditorMain
(
    const GameViewContext* gameContext
) :
    _gc(gameContext)
{
}


void EditorMain::onViewAdded(ove::ViewStack& stateController)
{
}

void EditorMain::onViewRemoved(ove::ViewStack& stateController)
{
}

void EditorMain::onViewStartFrame(ove::ViewStack& stateController)
{
}

void EditorMain::simulateView(ove::ViewStack& stateController, double dt)
{
}

void EditorMain::frameUpdateView
(
    ove::ViewStack& stateController,
    double dt,
    const cinek::uicore::InputState& inputState
)
{
    if (inputState.testKeyMod(KMOD_SHIFT)) {
        if (inputState.testKey(SDL_SCANCODE_A)) {
            stateController.present("EditorAddEntityToScene");
        }
    }
}

void EditorMain::onViewEndFrame(ove::ViewStack& stateController)
{
}
    
const char* EditorMain::viewId() const
{
    return "EditorMain";
}


} /* namespace cinek */