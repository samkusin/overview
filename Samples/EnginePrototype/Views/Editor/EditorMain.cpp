//
//  EditorMain.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/15/16.
//
//

#include "EditorMain.hpp"

#include "Engine/Render/RenderContext.hpp"
#include "Engine/ViewStack.hpp"
#include "UICore/UIBuilder.hpp"

namespace cinek {


EditorMain::EditorMain
(
    GameViewContext* gameContext
) :
    GameState(gameContext)
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
    layoutUI();
}

void EditorMain::onViewEndFrame(ove::ViewStack& stateController)
{
    handleUI(stateController);
}
    
const char* EditorMain::viewId() const
{
    return "EditorMain";
}

void EditorMain::layoutUI()
{
    //  layout UI
    uicore::Layout::Style style;
    style.mask = uicore::Layout::Style::has_margins;
    style.margins = { 8, 8, 8, 8 };
    
    uicore::Layout uiLayout;
    
    _frameState.keyEventLimit = sizeof(_frameKeyEvents) / sizeof(_frameKeyEvents[0]);
    _frameState.keyEvents = _frameKeyEvents;
    
    auto frameWidth = renderContext().frameRect.w;
    auto frameHeight = renderContext().frameRect.h;
    
    uiLayout.beginFrame(UI_BUTTON0_DOWN | UI_KEY_DOWN, &_frameState, nullptr, nullptr)
        .setSize(frameWidth, frameHeight);
    uiLayout.end();
    
    uiService().setKeyboardFocusToItem(_frameState.thisItem);
}

void EditorMain::handleUI(ove::ViewStack& stateController)
{
    uicore::KeyEvent keyEvt;
    while ((keyEvt = _frameState.popKey())) {
    
        //  Modifier Commands
        if (keyEvt.mod & KMOD_SHIFT) {
            
            //  Add Entity Mode
            if (keyEvt.key == SDL_SCANCODE_A) {
                stateController.present("EditorAddEntityToScene");
                break;
            }
            //  Switch to Play Mode
            else if (keyEvt.key == SDL_SCANCODE_TAB) {
                game().setGameMode(GameMode::kPlay);
                break;
            }
            
        }
        
    }
}


} /* namespace cinek */