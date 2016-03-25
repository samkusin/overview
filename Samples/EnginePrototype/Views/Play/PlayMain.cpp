//
//  PlayMain.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/18/16.
//
//

#include "PlayMain.hpp"

#include "Engine/Render/RenderContext.hpp"
#include "Engine/Path/Pathfinder.hpp"
#include "Engine/Controller/NavSystem.hpp"
#include "Engine/Path/NavPathQuery.hpp"
#include "Engine/ViewStack.hpp"


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

void PlayMain::simulateView(ove::ViewStack& stateController, CKTimeDelta dt)
{
}

void PlayMain::frameUpdateView
(
    ove::ViewStack& stateController,
    CKTimeDelta dt,
    const cinek::input::InputState& inputState
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
    /*
    //  layout UI
    uicore::Layout::Style style;
    style.mask = uicore::Layout::Style::has_margins;
    style.margins = { 8, 8, 8, 8 };
    
    uicore::Layout uiLayout;
    
    
    auto frameWidth = uiService().frame().width();
    auto frameHeight = uiService().frame().height();
    */
}

void PlayMain::handleUI(ove::ViewStack& stateController)
{
    /*
    uicore::KeyEvent keyEvt;
    while ((keyEvt = uiService().frame().popKey())) {
    
        if (keyEvt.mod & KMOD_SHIFT) {
        
            //  switch mode to editor
            if (keyEvt.key == SDL_SCANCODE_TAB) {
                game().setGameMode(GameMode::kEditor);
                break;
            }
        
        }
    }
    
    //  main frame selections
    if (uiService().frame().eventType() == UI_BUTTON0_DOWN) {
    
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
    */
}

}