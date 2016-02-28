//
//  PlayMain.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/18/16.
//
//

#include "PlayMain.hpp"

#include "Engine/Services/RenderService.hpp"
#include "Engine/Nav/Pathfinder.hpp"
#include "Engine/Nav/NavSystem.hpp"
#include "Engine/Nav/NavPathQuery.hpp"
#include "Engine/ViewStack.hpp"

#include "UICore/UIBuilder.hpp"

namespace cinek {

PlayMain::PlayMain(GameViewContext* gameContext) :
    GameState(gameContext)
{
}
    
void PlayMain::onViewAdded(ove::ViewStack& stateController)
{
    _focusedEntity = game().getFocusedGameEntity();
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
    
    auto frameWidth = renderService().getViewRect().w;
    auto frameHeight = renderService().getViewRect().h;
    
    uiLayout.beginFrame(UI_BUTTON0_DOWN | UI_KEY_DOWN, &_frameState, nullptr, nullptr)
        .setSize(frameWidth, frameHeight);
    uiLayout.end();
    
    uiService().setKeyboardFocusToItem(_frameState.thisItem);
}

void PlayMain::handleUI(ove::ViewStack& stateController)
{
    uicore::KeyEvent keyEvt;
    while ((keyEvt = _frameState.popKey())) {
    
        if (keyEvt.mod & KMOD_SHIFT) {
        
            //  switch mode to editor
            if (keyEvt.key == SDL_SCANCODE_TAB) {
                game().setGameMode(GameMode::kEditor);
                break;
            }
        
        }
    }
    
    //  main frame selections
    if (_frameState.evtType == UI_BUTTON0_DOWN) {
    
        auto& rayTestResult = sceneRayTestResult();
        if (rayTestResult) {
            ckm::vector3f pos;
            ckm::vector3f extents { ckm::scalar(0.00), ckm::scalar(0.05), ckm::scalar(0.0) };
            ove::ckmFromBt(pos, rayTestResult.position);
            
            auto query = pathfinder().acquireQuery();
            if (query) {
                query->setupFilters(ove::kNavMeshPoly_Walkable);
                if (query->isWalkable(pos, extents)) {
                    navSystem().moveBodyToPosition(_focusedEntity, pos);
                }
            }
        }
    }
}

}