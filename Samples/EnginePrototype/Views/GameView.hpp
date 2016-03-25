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

class GameView : public AppViewController, GameInterface
{
public:
    GameView(ApplicationContext* context);
    
private:
    virtual void setGameMode(GameMode mode) override;
    virtual GameMode getGameMode() const override;
    virtual Entity getFocusedGameEntity() const override;
    
private:
    //  ViewController
    virtual void onViewAdded(ove::ViewStack& stateController) override;
    virtual void onViewRemoved(ove::ViewStack& stateController) override;
    virtual void onViewStartFrame(ove::ViewStack& stateController) override;
    virtual void simulateView(ove::ViewStack& stateController, CKTimeDelta dt) override;
    virtual void frameUpdateView(ove::ViewStack& stateController, CKTimeDelta dt,
                                 const cinek::input::InputState& inputState) override;
    virtual void onViewEndFrame(ove::ViewStack& stateController) override;
        
    virtual const char* viewId() const override;

private:
    gfx::NodeRenderer _renderer;
    gfx::Camera _camera;
    ove::SceneRayTestResult _viewToSceneRayTestResult;
    
    ove::ViewStack _viewStack;
    
    GameViewContext _gameViewContext;
    GameMode _gameMode;
    Entity _focusedEntity;
    
    std::shared_ptr<EditorView> _editorView;
    std::shared_ptr<PlayView> _playView;
};

} /* namespace cinek */

#endif /* Sample_Engine_GameView_hpp */
