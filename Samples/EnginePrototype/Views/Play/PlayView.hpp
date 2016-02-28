//
//  PlayView.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/18/16.
//
//

#ifndef Prototype_Game_PlayView_hpp
#define Prototype_Game_PlayView_hpp

#include "Engine/ViewController.hpp"
#include "Engine/ViewStack.hpp"

#include "GameViewContext.hpp"

namespace cinek {

class PlayMain;

class PlayView : public GameState
{
public:
    PlayView(GameViewContext* gameContext);
    
private:
    //  ViewController
    virtual void onViewAdded(ove::ViewStack& stateController) override;
    virtual void onViewRemoved(ove::ViewStack& stateController) override;
    virtual void onViewStartFrame(ove::ViewStack& stateController) override;
    virtual void simulateView(ove::ViewStack& stateController, double dt) override;
    virtual void frameUpdateView(ove::ViewStack& stateController, double dt,
                                 const cinek::uicore::InputState& inputState) override;
    virtual void onViewEndFrame(ove::ViewStack& stateController) override;
    
    virtual const char* viewId() const override;

private:
    ove::ViewStack _viewStack;
    
    std::shared_ptr<PlayMain> _mainState;
};

}

#endif /* Prototype_Game_PlayView_hpp */
