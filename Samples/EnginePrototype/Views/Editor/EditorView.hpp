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

#include "Views/ViewStateMachine.hpp"

namespace cinek {

class EditorView : public GameState
{
public:
    EditorView(GameViewContext* gameContext);
    virtual ~EditorView();

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
    Allocator _allocator;
    ove::FreeCameraController _freeCameraController;
    
private:
    void initUIData();
    
    std::vector<std::string> _entityNamespaces;
    
    struct EntityTemplate
    {
        int nsIndex;
        const char* name;
        const char* longName;
        
        explicit operator bool() const {
            return name != nullptr;
        }
        void reset() {
            nsIndex = -1;
            name = nullptr;
            longName = nullptr;
        }
    };
    
    struct EntityCategory
    {
        std::vector<EntityTemplate> names;
        CStringStack strstack;
        int size = 0;
        
        EntityCategory() = default;
        EntityCategory(int cnt, int maxstrlen);
    };
    
    std::vector<std::pair<std::string, EntityCategory>> _entityCategories;
    
    class SceneTree;
    struct SceneTreeNode;
    unique_ptr<SceneTree> _sceneTree;
    void rebuildSceneTree(SceneTree& tree);
   
    //  state based data
    Entity _stagedEntity;
    
private:
    //  states
    enum
    {
        kStateId_Idle,
        kStateId_AddEntityToScene
    };

    ove::ViewStateMachine _vsm;

    void setAddEntityToSceneState();
    void setIdleState();
    
    struct UIStatus
    {
        EntityTemplate addEntityTemplate;
    
        bool displayMainUI;
    };
    
    UIStatus _uiStatus;
    
    void updateUI(UIStatus& status, float width, float height);
    void updateSceneTreeUI(const SceneTreeNode* node);
};

}   /* namespace cinek */

#endif /* Prototype_Game_EditorView_hpp */
