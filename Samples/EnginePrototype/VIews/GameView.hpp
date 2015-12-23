//
//  GameView.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

#ifndef Sample_Engine_GameView_hpp
#define Sample_Engine_GameView_hpp

#include "AppViewController.hpp"

#include <cinek/allocator.hpp>

namespace cinek {

class GameView : public AppViewController
{
public:
    GameView(const ApplicationContext& context);
    
    virtual void onViewAdded(ove::ViewStack& stateController);
    virtual void onViewRemoved(ove::ViewStack& stateController);
    virtual void onViewForeground(ove::ViewStack& stateController);
    virtual void onViewBackground(ove::ViewStack& stateController);
    virtual void onViewStartFrame(ove::ViewStack& stateController);
    virtual void simulateView(ove::ViewStack& stateController, double dt);
    virtual void frameUpdateView(ove::ViewStack& stateController, double dt);
    virtual void onViewEndFrame(ove::ViewStack& stateController);
    
    virtual const char* viewId() const;
    
};

} /* namespace cinek */

#endif /* Sample_Engine_GameView_hpp */
