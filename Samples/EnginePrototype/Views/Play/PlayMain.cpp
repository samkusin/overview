//
//  PlayMain.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/18/16.
//
//

#include "PlayMain.hpp"

#include "Engine/Services/RenderService.hpp"
#include "Engine/ViewStack.hpp"

#include "UICore/UIBuilder.hpp"

namespace cinek {

PlayMain::PlayMain(GameViewContext* gameContext) :
    _gc(gameContext)
{
}
    
void PlayMain::onViewAdded(ove::ViewStack& stateController)
{
}

void PlayMain::onViewRemoved(ove::ViewStack& stateController)
{
}

void PlayMain::onViewStartFrame(ove::ViewStack& stateController)
{
}

void PlayMain::simulateView(ove::ViewStack& stateController, double dt)
{
}

void PlayMain::frameUpdateView
(
    ove::ViewStack& stateController,
    double dt,
    const cinek::uicore::InputState& inputState
)
{
    layoutUI();
}

void PlayMain::onViewEndFrame(ove::ViewStack& stateController)
{
    handleUI(stateController);
}
    
const char* PlayMain::viewId() const
{
    return "PlayMain";
}

void PlayMain::layoutUI()
{
    //  layout UI
    uicore::Layout::Style style;
    style.mask = uicore::Layout::Style::has_margins;
    style.margins = { 8, 8, 8, 8 };
    
    uicore::Layout uiLayout;
    
    _frameState.keyEventLimit = sizeof(_frameKeyEvents) / sizeof(_frameKeyEvents[0]);
    _frameState.keyEvents = _frameKeyEvents;
    
    auto frameWidth = _gc->renderService->getViewRect().w;
    auto frameHeight = _gc->renderService->getViewRect().h;
    
    uiLayout.beginFrame(UI_BUTTON0_DOWN | UI_KEY_DOWN, &_frameState, nullptr, nullptr)
        .setSize(frameWidth, frameHeight);
    uiLayout.end();
    
    _gc->uiService->setKeyboardFocusToItem(_frameState.thisItem);
}

void PlayMain::handleUI(ove::ViewStack& stateController)
{
    uicore::KeyEvent keyEvt;
    while ((keyEvt = _frameState.popKey())) {
    
        if (keyEvt.mod & KMOD_SHIFT) {
            if (keyEvt.key == SDL_SCANCODE_TAB) {
                _gc->setMode(GameViewContext::Mode::kEditor);
                break;
            }
        }
        
    }
}


}