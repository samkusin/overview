//
//  EditorView.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/15/16.
//
//

#ifndef Prototype_Game_EditorView_hpp
#define Prototype_Game_EditorView_hpp

#include "Engine/ViewController.hpp"
#include "Engine/ViewStack.hpp"
#include "CKGfx/RenderTarget.hpp"
#include "CKGfx/Texture.hpp"
#include "CKGfx/Mesh.hpp"
#include "CKGfx/Camera.hpp"

#include "GameViewContext.hpp"
#include "FreeCameraController.hpp"

#include <cinek/allocator.hpp>
#include <vector>

namespace cinek {

class EditorMain;
class EditorAddEntityToScene;

class EditorView : public ove::ViewController,
                   public uicore::DataProvider
{
public:
    EditorView(GameViewContext* gameContext);

private:
    //  ViewController
    virtual void onViewAdded(ove::ViewStack& stateController) override;
    virtual void onViewRemoved(ove::ViewStack& stateController) override;;
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
    GameViewContext* _gc;
    ove::ViewStack _viewStack;
    
    ove::FreeCameraController _freeCameraController;
    
    std::shared_ptr<EditorMain> _mainState;
    std::shared_ptr<EditorAddEntityToScene> _addEntityToSceneState;
    
    /*
    //  Node graph to render to our dedicated
    unique_ptr<gfx::NodeGraph> _modelStageGraph;
    gfx::Camera _modelStageCamera;
    gfx::MultiTextureRenderTarget _defaultEntityTemplateRT;
    */
    
private:
    gfx::Mesh _testQuadMesh;
    gfx::Mesh _testSphereMesh;

    void test1();
    void test2();
};

}   /* namespace cinek */

#endif /* Prototype_Game_EditorView_hpp */
