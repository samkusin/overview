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
#include <cinek/cstringstack.hpp>
#include <vector>
#include <string>

namespace cinek {

class EditorMain;
class EditorAddEntityToScene;

class EditorView : public GameState
{
public:
    EditorView(GameViewContext* gameContext);

private:
    //  ViewController
    virtual void onViewAdded(ove::ViewStack& stateController) override;
    virtual void onViewRemoved(ove::ViewStack& stateController) override;;
    virtual void onViewStartFrame(ove::ViewStack& stateController) override;
    virtual void simulateView(ove::ViewStack& stateController, CKTimeDelta dt) override;
    virtual void frameUpdateView(ove::ViewStack& stateController, CKTimeDelta dt,
                                 const cinek::input::InputState& inputState) override;
    virtual void onViewEndFrame(ove::ViewStack& stateController) override;
    
    virtual const char* viewId() const override;
    

private:
    ove::ViewStack _viewStack;
    
    ove::FreeCameraController _freeCameraController;
    
    std::shared_ptr<EditorMain> _mainState;
    std::shared_ptr<EditorAddEntityToScene> _addEntityToSceneState;
    
private:
    void initUIData();
    void layoutUI(float width, float height);

    
    struct EntityCategory
    {
        std::vector<const char*> names;
        CStringStack strstack;
        
        EntityCategory();
        EntityCategory(int cnt, int maxstrlen);
    };
    
    std::vector<std::pair<std::string, EntityCategory>> _entityCategories;
    
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
