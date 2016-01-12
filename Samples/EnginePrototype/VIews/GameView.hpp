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
    virtual bool onUIDataItemRequest(int id, uint32_t row, uint32_t col,
        uicore::DataObject& data) override;
    virtual uint32_t onUIDataItemRowCountRequest(int id) override;
    
    //  FrameHandler
    virtual void onUIFrameEvent(int id, const uicore::FrameEvent& evt) override;
    
private:
    gfx::NodeRenderer _renderer;
    gfx::Camera _camera;
    ove::FreeCameraController _freeCameraController;
    bool _sceneLoaded;

private:
    enum
    {
        kUIEvtId_GameView
    };
    
    enum
    {
        kUIProviderId_EntityTemplates
    };
    
    bool _shiftModifierAction;
    
    //  Node graph to render to our dedicated
    unique_ptr<gfx::NodeGraph> _modelStageGraph;
    gfx::Camera _modelStageCamera;
    
    //  Storage for UI
    struct EntityTemplateUIData
    {
        std::string name;
        std::string longname;
    };
    
    std::vector<EntityTemplateUIData> _entityTemplateUIList;
    gfx::MultiTextureRenderTarget _defaultEntityTemplateRT;
    int _selectedEntityTemplateIndex;

    void addEntityTemplateUIData(std::string name, const JsonValue& entityTemplate);
    
private:
    gfx::Mesh _testQuadMesh;

    void test1();
};

} /* namespace cinek */

#endif /* Sample_Engine_GameView_hpp */
