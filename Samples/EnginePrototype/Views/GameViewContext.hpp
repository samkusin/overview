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
    ove::Scene* scene;
    ove::EntityService* entityService;
    ove::AssetService* assetService;
    ove::RenderContext* renderContext;
    NVGcontext* nvgContext;
    ove::Pathfinder* pathfinder;
    ove::PathfinderDebug* pathfinderDebug;
    ove::NavSystem* navSystem;
    
    GameInterface* game;
};

class GameState : public ove::ViewController
{
public:
    GameState(GameViewContext* context);
    
    GameInterface& game() { return *_context->game; }
    const GameInterface& game() const { return *_context->game; }
    gfx::Camera& camera() { return *_context->camera; };
    ove::Scene& scene() { return *_context->scene; }
    ove::EntityService& entityService() { return *_context->entityService; }
    ove::AssetService& assetSevice() { return *_context->assetService; }
    ove::Pathfinder& pathfinder() { return *_context->pathfinder; }
    ove::PathfinderDebug& pathfinderDebug() { return *_context->pathfinderDebug; }
    ove::NavSystem& navSystem() { return *_context->navSystem; }
    ove::RenderContext& renderContext() { return *_context->renderContext; }
    const ove::SceneRayTestResult& sceneRayTestResult() const { return *_context->screenRayTestResult; }
    
private:
    GameViewContext* _context;
};

} /* namespace cinek */


#endif /* Prototype_GameViewContext_hpp */
