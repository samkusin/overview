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

#include "FreeCameraController.hpp"


#include "CKGfx/Camera.hpp"
#include "CKGfx/RenderTarget.hpp"
#include "CKGfx/Texture.hpp"
#include "CKGfx/Mesh.hpp"

#include <cinek/allocator.hpp>
#include <vector>

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
                         const ove::InputState& inputState) override;
    

    //  DataProvider
    virtual bool onUIDataItemRequest(int id, int row, int col, uicore::DataObject& data) override;

    
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
    
    //  Node graph to render to our dedicated
    gfx::NodeGraph _modelGraph;
    gfx::Camera _modelStageCamera;
    void createModelStage();
    
    //  Storage for UI
    struct EntityTemplateUIData
    {
        gfx::MultiTextureRenderTarget rt;
        std::string name;
    };
    
    std::vector<EntityTemplateUIData> _entityTemplateUIList;
    int _selectedEntityTemplateIndex;
    
private:
    void test1();
    gfx::MultiTextureRenderTarget _testTarget;
    gfx::Mesh _testQuadMesh;
};

} /* namespace cinek */

#endif /* Sample_Engine_GameView_hpp */
