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

class GameView : public AppViewController, uicore::DataProvider, uicore::FrameHandler
{
public:
    GameView(const ApplicationContext& context);
    
    virtual void onViewAdded(ove::ViewStack& stateController) override;
    virtual void onViewRemoved(ove::ViewStack& stateController) override;
    virtual void onViewForeground(ove::ViewStack& stateController) override;
    virtual void onViewBackground(ove::ViewStack& stateController) override;
    virtual void onViewStartFrame(ove::ViewStack& stateController) override;
    virtual void simulateView(ove::ViewStack& stateController, double dt) override;
    virtual void onViewEndFrame(ove::ViewStack& stateController) override;
        
    virtual const char* viewId() const override;
    
private:
    static void viewUIRenderHook(void* context, NVGcontext* nvg);

    //  AppViewController
    virtual void frameUpdateView(ove::ViewStack& stateController, double dt,
                         const ove::InputState& inputState,
                         ove::RenderService& renderService) override;
    

    //  DataProvider
    virtual void onUIDataItemRequest(int id, int index, uicore::DataObject& data) override;

    
    //  FrameHandler
    virtual void onUIFrameEvent(int id, const uicore::FrameEvent& evt) override;
    
private:
    gfx::NodeRenderer _renderer;
    gfx::Camera _camera;
    ove::FreeCameraController _freeCameraController;
    
    
private:

    enum
    {
        kUIEvtId_GameView
    };
    
    enum
    {
        kUIProviderId_EntityTemplates
    };
    
    int _selectedEntityTemplateIndex;
};

} /* namespace cinek */

#endif /* Sample_Engine_GameView_hpp */
