//
//  GameView.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

#ifndef Sample_Engine_GameView_hpp
#define Sample_Engine_GameView_hpp

#include "Views/AppViewController.hpp"
#include "Engine/ViewStack.hpp"
#include "Engine/Physics/SceneTypes.hpp"
#include "CKGfx/Camera.hpp"

#include "GameViewContext.hpp"

#include <cinek/allocator.hpp>

namespace cinek {

class GameView : public AppViewController
{
public:
    GameView(const ApplicationContext* context);
    
private:
    //  ViewController
    virtual void onViewAdded(ove::ViewStack& stateController) override;
    virtual void onViewRemoved(ove::ViewStack& stateController) override;
    virtual void onViewStartFrame(ove::ViewStack& stateController) override;
    virtual void simulateView(ove::ViewStack& stateController, double dt) override;
    virtual void frameUpdateView(ove::ViewStack& stateController, double dt,
                                 const cinek::uicore::InputState& inputState) override;
    virtual void onViewEndFrame(ove::ViewStack& stateController) override;
        
    virtual const char* viewId() const override;

private:
    gfx::NodeRenderer _renderer;
    gfx::Camera _camera;
    ove::SceneRayTestResult _viewToSceneRayTestResult;
    
    ove::ViewStack _viewStack;
    
    GameViewContext _gameViewContext;
    
    std::shared_ptr<EditorView> _editorView;
};

} /* namespace cinek */

#endif /* Sample_Engine_GameView_hpp */
