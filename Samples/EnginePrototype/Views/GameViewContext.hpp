//
//  GameViewContext.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/15/16.
//
//

#ifndef Prototype_GameViewContext_hpp
#define Prototype_GameViewContext_hpp

#include "GameTypes.hpp"
#include "Engine/Physics/SceneTypes.hpp"
#include "Engine/ViewController.hpp"
#include "UICore/Input.hpp"
#include "UICore/UITypes.hpp"
#include "CKGfx/GfxTypes.hpp"

namespace cinek {

class LoadSceneView;
class EditorView;
class PlayView;

enum class GameMode
{
    kNone,
    kPlay,
    kEditor
};


class GameInterface
{
public:
    virtual void setGameMode(GameMode mode) = 0;
    virtual GameMode getGameMode() const = 0;
    virtual Entity getFocusedGameEntity() const = 0;
};

struct GameViewContext
{
    gfx::Camera* camera;
    
    ove::SceneRayTestResult* screenRayTestResult;
    ove::SceneService* sceneService;
    ove::EntityService* entityService;
    ove::RenderService* renderService;
    ove::AssetService* assetService;
    
    UIService* uiService;
    NVGcontext* nvgContext;
    ove::Pathfinder* pathfinder;
    ove::PathfinderDebug* pathfinderDebug;
    
    GameInterface* game;
};

class GameState : public ove::ViewController
{
public:
    GameState(GameViewContext* context);
    
protected:
    GameInterface& game() { return *_context->game; }
    const GameInterface& game() const { return *_context->game; }
    gfx::Camera& camera() { return *_context->camera; };
    ove::SceneService& sceneService()  { return *_context->sceneService; }
    ove::EntityService& entityService() { return *_context->entityService; }
    ove::RenderService& renderService() { return *_context->renderService; }
    ove::AssetService& assetSevice() { return *_context->assetService; }
    ove::Pathfinder& pathfinder() { return *_context->pathfinder; }
    ove::PathfinderDebug& pathfinderDebug() { return *_context->pathfinderDebug; }
    
    const ove::SceneRayTestResult& sceneRayTestResult() const { return *_context->screenRayTestResult; }
    UIService& uiService() { return *_context->uiService; }
    
private:
    GameViewContext* _context;
};

} /* namespace cinek */


#endif /* Prototype_GameViewContext_hpp */
