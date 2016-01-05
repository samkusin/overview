//
//  StartupView.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

#ifndef Sample_Engine_StartupView_hpp
#define Sample_Engine_StartupView_hpp

#include "AppViewController.hpp"

namespace cinek {

class StartupView : public AppViewController
{
public:
    StartupView(const ApplicationContext& context);
    
    virtual void onViewAdded(ove::ViewStack& stateController);
    virtual void onViewRemoved(ove::ViewStack& stateController);
    virtual void onViewForeground(ove::ViewStack& stateController);
    virtual void onViewBackground(ove::ViewStack& stateController);
 
    virtual void onViewStartFrame(ove::ViewStack& stateController);
    virtual void simulateView(ove::ViewStack& stateController, double dt);
    virtual void onViewEndFrame(ove::ViewStack& stateController);
    
    virtual const char* viewId() const;
    
private:
    //  AppViewController
    virtual void frameUpdateView(ove::ViewStack& stateController, double dt,
                         const ove::InputState& inputState,
                         ove::RenderService& renderService);
private:
    bool _loadCompleted;
    
};

} /* namespace cinek */

#endif /* Sample_Engine_StartupView_hpp */
