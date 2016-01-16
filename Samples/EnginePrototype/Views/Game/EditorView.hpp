//
//  EditorView.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/15/16.
//
//

#ifndef Prototype_Game_EditorView_hpp
#define Prototype_Game_EditorView_hpp

#include "Views/AppViewController.hpp"
#include "Engine/ViewStack.hpp"
#include "CKGfx/RenderTarget.hpp"
#include "CKGfx/Texture.hpp"
#include "CKGfx/Mesh.hpp"

#include "GameViewContext.hpp"
#include "FreeCameraController.hpp"

#include <cinek/allocator.hpp>
#include <vector>

namespace cinek {

class EditorView : public AppViewController,
                   public uicore::DataProvider
{
public:
    EditorView(const ApplicationContext& context,
               const GameViewContext& gameContext);

private:
    //  ViewController
    virtual void onViewAdded(ove::ViewStack& stateController) override;
    virtual void onViewRemoved(ove::ViewStack& stateController) override;
    virtual void onViewForeground(ove::ViewStack& stateController) override;
    virtual void onViewBackground(ove::ViewStack& stateController) override;
    virtual void onViewStartFrame(ove::ViewStack& stateController) override;
    virtual void simulateView(ove::ViewStack& stateController, double dt) override;
    virtual void frameUpdateView(ove::ViewStack& stateController, double dt,
                                 const cinek::uicore::InputState& inputState) override;
    virtual void onViewEndFrame(ove::ViewStack& stateController) override;
    
    virtual const char* viewId() const override;
    
    //  DataProvider
    virtual bool onUIDataItemRequest(int id, uint32_t row, uint32_t col,
        uicore::DataObject& data) override;
    virtual uint32_t onUIDataItemRowCountRequest(int id) override;

private:
    enum
    {
        kUIProviderId_EntityTemplates
    };

    static void viewUIRenderHook(void* context, NVGcontext* nvg);

    void createAndStageEntity(EntityContextType storeId,
        const std::string& ns, const std::string& name);
    
    enum class UnstageOption { kPlace, kDiscard };
    void unstageEntity(UnstageOption option);
    
    void updateStagedEntity(const ove::SceneRayTestResult& hitResult);

private:
    const GameViewContext* _gameContext;
    ove::FreeCameraController _freeCameraController;
    
    uicore::FrameState _sceneFrameState;
    bool _shiftModifierAction;
    bool _displayTemplateSelector;

private:
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
    
    //  used for staging an entity for placement into the active scene
    uicore::ListboxState _entityTemplateListboxState;
    Entity _stagedEntity;

    void addEntityTemplateUIData(std::string name, const JsonValue& entityTemplate);
    
private:
    gfx::Mesh _testQuadMesh;
    gfx::Mesh _testSphereMesh;

    void test1();
    void test2();
};

}   /* namespace cinek */

#endif /* Prototype_Game_EditorView_hpp */
