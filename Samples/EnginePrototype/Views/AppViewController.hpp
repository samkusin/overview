//
//  AppViewController.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/22/15.
//
//

#ifndef Prototype_AppViewController_hpp
#define Prototype_AppViewController_hpp

#include "GameTypes.hpp"
#include "Common.hpp"
#include "UICore/UITypes.hpp"
#include "CKGfx/NodeRenderer.hpp"
#include "UICore/Input.hpp"
#include "Engine/ViewController.hpp"
#include "Engine/Messages/Core.hpp"
#include "Engine/Services/AssetService.hpp"
#include "Engine/Services/EntityService.hpp"
#include "Engine/Services/SceneService.hpp"
#include "Engine/Services/RenderService.hpp"
#include "Engine/Services/PathfinderService.hpp"

namespace cinek {

struct ApplicationContext
{
    NVGcontext* nvg;
    TaskScheduler* taskScheduler;
    ove::MessageClientSender* msgClientSender;
    ove::AssetManfiestFactory* resourceFactory;
    ove::EntityDatabase* entityDatabase;
    gfx::Context* gfxContext;
    ove::SceneDebugDrawer* sceneDebugDrawer;
    ove::SceneDataContext* sceneData;
    ove::Scene* scene;
    ove::RenderGraph* renderGraph;
    ove::RenderContext* renderContext;
    ove::Pathfinder* pathfinder;
    ove::PathfinderDebug* pathfinderDebug;
};

class AppViewController : public ove::ViewController
{
public:
    AppViewController(ApplicationContext* context);
    
    virtual ~AppViewController();
    
protected:
    ove::AssetService& assetService() {
        return _assetService;
    }
    ove::EntityService& entityService() {
        return _entityService;
    }
    ove::SceneService& sceneService() {
        return _sceneService;
    }
    ove::RenderService& renderService() {
        return _renderService;
    }
    ove::PathfinderService& pathfinderService() {
        return _pathfinderService;
    }
    NVGcontext* nvgContext() {
        return _appContext->nvg;
    }
    
private:
    ApplicationContext* _appContext;
    ove::AssetService _assetService;
    ove::EntityService _entityService;
    ove::SceneService _sceneService;
    ove::RenderService _renderService;
    ove::PathfinderService _pathfinderService;
};

}



#endif /* Prototype_AppViewController_hpp */
