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

#include "Engine/ViewController.hpp"
#include "Engine/Services/AssetService.hpp"
#include "Engine/Services/EntityService.hpp"
#include "Engine/Services/SceneService.hpp"

namespace cinek {

struct ApplicationContext
{
    TaskScheduler* taskScheduler;
    ove::MessageClientSender* msgClientSender;
    ove::AssetManfiestFactory* resourceFactory;
    ove::EntityDatabase* entityDatabase;
    gfx::Context* gfxContext;
    ove::SceneDataContext* sceneData;
    ove::Scene* scene;
    ove::RenderGraph* renderGraph;
};


class AppViewController : public ove::ViewController
{
public:
    AppViewController(const ApplicationContext& context);
    
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
private:
    ApplicationContext _appContext;
    ove::AssetService _assetService;
    ove::EntityService _entityService;
    ove::SceneService _sceneService;
};

}



#endif /* Prototype_AppViewController_hpp */
