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
#include "Engine/ViewController.hpp"
#include "Engine/Services/AssetService.hpp"
#include "Engine/Services/EntityService.hpp"
#include "Engine/Services/SceneService.hpp"
#include "Engine/Services/RenderService.hpp"

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
};

class AppViewController : public ove::ViewController
{
public:
    AppViewController(const ApplicationContext& context);
    
    virtual ~AppViewController();
    
    virtual void frameUpdateView(ove::ViewStack& stateController, double dt,
                                 const ove::InputState& inputState) = 0;
    
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
    NVGcontext* nvgContext() {
        return _appContext->nvg;
    }
    
private:
    const ApplicationContext* _appContext;
    ove::AssetService _assetService;
    ove::EntityService _entityService;
    ove::SceneService _sceneService;
    ove::RenderService _renderService;
};

}



#endif /* Prototype_AppViewController_hpp */
