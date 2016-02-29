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
#include "Engine/Services/RenderService.hpp"

namespace cinek {

struct ApplicationContext
{
    NVGcontext* nvg;
    TaskScheduler* taskScheduler;
    UIContext *uiContext;
    
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
    ove::NavSystem* navSystem;
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
    ove::RenderService& renderService() {
        return _renderService;
    }
    ove::Scene& scene() {
        return *_appContext->scene;
    }
    ove::SceneDebugDrawer& sceneDebug() {
        return *_appContext->sceneDebugDrawer;
    }
    ove::Pathfinder& pathfinder() {
        return *_pathfinder;
    }
    ove::PathfinderDebug& pathfinderDebug() {
        return *_pathfinderDebug;
    }
    ove::NavSystem& navSystem() {
        return *_navSystem;
    }
    UIService& uiService() {
        return _uiService;
    }
    NVGcontext* nvgContext() {
        return _appContext->nvg;
    }
    TaskScheduler& scheduler() {
        return *_appContext->taskScheduler;
    }
    ove::RenderGraph& renderGraph() {
        return *_appContext->renderGraph;
    }
    gfx::Context& gfxContext() {
        return *_appContext->gfxContext;
    }

private:
    ApplicationContext* _appContext;
    ove::AssetService _assetService;
    ove::EntityService _entityService;
    ove::RenderService _renderService;
    
    ove::Pathfinder* _pathfinder;
    ove::PathfinderDebug* _pathfinderDebug;
    ove::NavSystem* _navSystem;
    
    UIService _uiService;
};

}



#endif /* Prototype_AppViewController_hpp */
