//
//  EditorMain.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/15/16.
//
//

#ifndef Prototype_Game_Editor_Main_hpp
#define Prototype_Game_Editor_Main_hpp

#include "Engine/ViewController.hpp"

#include "GameViewContext.hpp"

#include "UICore/UITypes.hpp"


namespace cinek {

class EditorMain : public GameState
{
public:
    EditorMain(GameViewContext* gameContext);
    
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
    uicore::FrameState _frameState;
    uicore::KeyEvent _frameKeyEvents[16];

    void layoutUI();
    void handleUI(ove::ViewStack& stateController);
};

} /* namespace cinek */

#endif