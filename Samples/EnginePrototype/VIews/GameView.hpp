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
#include "UICore/UITypes.hpp"
#include "CKGfx/Camera.hpp"

#include "FreeCameraController.hpp"

#include <cinek/allocator.hpp>

namespace cinek {

class GameView : public AppViewController, uicore::UISubscriber
{
public:
    GameView(const ApplicationContext& context);
    
    virtual void onViewAdded(ove::ViewStack& stateController);
    virtual void onViewRemoved(ove::ViewStack& stateController);
    virtual void onViewForeground(ove::ViewStack& stateController);
    virtual void onViewBackground(ove::ViewStack& stateController);
    virtual void onViewStartFrame(ove::ViewStack& stateController);
    virtual void simulateView(ove::ViewStack& stateController, double dt);
    virtual void onViewEndFrame(ove::ViewStack& stateController);
        
    virtual const char* viewId() const;
    
private:
    static void viewUIRenderHook(void* context, NVGcontext* nvg);

    //  AppViewController
    virtual void frameUpdateView(ove::ViewStack& stateController, double dt,
                         const ove::InputState& inputState,
                         ove::RenderService& renderService);
    

    //  UISubscriber
    virtual void onUIEvent(int evtId, UIevent evtType, const uicore::UIeventdata& data);
    
    
private:
    enum
    {
        kUIEvtId_GameView
    };
    
    gfx::NodeRenderer _renderer;
    gfx::Camera _camera;
    ove::FreeCameraController _freeCameraController;
};

} /* namespace cinek */

#endif /* Sample_Engine_GameView_hpp */
