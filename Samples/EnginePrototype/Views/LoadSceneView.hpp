//
//  LoadSceneView.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/8/16.
//
//

#ifndef Prototype_LoadSceneView_hpp
#define Prototype_LoadSceneView_hpp

#include "AppViewController.hpp"
#include "Engine/SceneJsonLoader.hpp"
#include "GameViewContext.hpp"

#include <vector>

namespace cinek {

class LoadSceneView : public AppViewController
{
public:
    LoadSceneView(ApplicationContext* appContext);

private:
    //  ViewController
    virtual void onViewAdded(ove::ViewStack& stateController) override;
    virtual void onViewRemoved(ove::ViewStack& stateController) override;;
    virtual void onViewStartFrame(ove::ViewStack& stateController) override;
    virtual void simulateView(ove::ViewStack& stateController, CKTimeDelta dt) override;
    virtual void frameUpdateView(ove::ViewStack& stateController, CKTimeDelta dt,
                                 const cinek::uicore::InputState& inputState) override;
    virtual void onViewEndFrame(ove::ViewStack& stateController) override;
    
    virtual const char* viewId() const override;
    
private:
    enum State
    {
        kLoadStart,
        kLoadManifest,
        kLoadScene,
        kLoadPaths,
        kLoadError,
        kLoadSuccess
    };
    
    State _currentTask;
    State _nextTask;
    
    std::shared_ptr<ove::AssetManifest> _manifest;
    ove::SceneJsonLoader _loader;
};

}   /* namespace cinek */

#endif /* LoadSceneView_hpp */
