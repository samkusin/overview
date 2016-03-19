//
//  PlayMain.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/18/16.
//
//

#ifndef Prototype_Game_PlayMain_hpp
#define Prototype_Game_PlayMain_hpp

#include "Engine/ViewController.hpp"

#include "GameViewContext.hpp"

namespace cinek {

class PlayMain;

class PlayMain : public GameState
{
public:
    PlayMain(GameViewContext* gameContext);
    
private:
    //  ViewController
    virtual void onViewAdded(ove::ViewStack& stateController) override;
    virtual void onViewRemoved(ove::ViewStack& stateController) override;
    virtual void onViewStartFrame(ove::ViewStack& stateController) override;
    virtual void simulateView(ove::ViewStack& stateController, CKTimeDelta dt) override;
    virtual void frameUpdateView(ove::ViewStack& stateController, CKTimeDelta dt,
                                 const cinek::uicore::InputState& inputState) override;
    virtual void onViewEndFrame(ove::ViewStack& stateController) override;
    
    virtual const char* viewId() const override;

private:
    Entity _focusedEntity;
    
    void layoutUI();
    void handleUI(ove::ViewStack& stateController);
};

}

#endif /* Prototype_Game_PlayMain_hpp */