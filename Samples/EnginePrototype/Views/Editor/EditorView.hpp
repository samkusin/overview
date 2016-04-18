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
    gfx::Matrix4 _savedCameraMtx;
    ove::FreeCameraController _freeCameraController;
    
private:
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
    
    Entity _activeEntity;       // the currently active entity (ui props, etc.)
    Entity _stagedEntity;       // a pending entity used for adding to scenes
    
    void createUIData();
    void renderOverlay();
    void destroyUIData();
    
private:
    struct UIStatus
    {
        EntityTemplate addEntityTemplate;
    
        bool displayMainUI;
        
        enum
        {
            kCamera_Perspective,
            kCamera_Orthogonal
        };
        int cameraOption;
        
        enum
        {
            kCameraDirection_Current,
            kCameraDirection_XYZPos,
            kCameraDirection_XYZNeg,
            kCameraDirection_XZYPos,
            kCameraDirection_XZYNeg,
            kCameraDirection_YZXPos,
            kCameraDirection_YZXNeg
        };
        int cameraDirection;
    };
    
    UIStatus _uiStatus;\
    
    void updateMainUI(UIStatus& status, float width, float height);
    void updateSceneTreeUI(const SceneTreeNode* node);
    void endFrameMainUI(UIStatus& status);

    struct UITransformStatus
    {
        enum
        {
            kTransform_Snap,
            kTransform_Free,
            kTransform_X,
            kTransform_Y,
            kTransform_Z,
            kTransform_Count
        };
        
        int option;

        enum
        {
            kSystem_Global,
            kSystem_Local
        };
        
        int system;
        
        enum class Mode
        {
            kTransform_Location,
            kTransform_Orientation
        };
        
        Mode mode;
        
        ckm::vector3 lastMouseDir;
    };
    
    UITransformStatus _uiTransformStatus;
    
    ckm::vector3 calcMouseDelta(const ckm::matrix4& transform,
        float z,
        float halfw, float halfh,
        float dx, float dy);
    
    ckm::scalar calcMouseRotation(ckm::vector3& direction,
        const ckm::matrix4& viewMtx,
        float mx, float my);
    
    void updateTransformUI(UITransformStatus& status);
    void handleTransformUI(UITransformStatus& status);
    void renderTransformUI(UITransformStatus& status);


    
     //  states
    enum
    {
        kStateId_Idle,
        kStateId_AddEntityToScene,
        kStateId_TransformEntity
    };

    ove::ViewStateMachine _vsm;

    void setIdleState();
    void setAddEntityToSceneState();
    void setTransformEntityState(UITransformStatus::Mode mode);
};

}   /* namespace cinek */

#endif /* Prototype_Game_EditorView_hpp */
