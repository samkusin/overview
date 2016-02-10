//
//  LoadSceneView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/8/16.
//
//

#include "LoadSceneView.hpp"
#include "Engine/Services/AssetService.hpp"
#include "Engine/Services/SceneService.hpp"
#include "Engine/Services/PathfinderService.hpp"
#include "Engine/ViewStack.hpp"

namespace cinek {

LoadSceneView::LoadSceneView(ApplicationContext* context) :
    AppViewController(context),
    _currentTask(kLoadStart),
    _nextTask(kLoadStart)
{
}


void LoadSceneView::onViewAdded(ove::ViewStack& stateController)
{
    //  load scene
    _currentTask = kLoadStart;
    _nextTask = kLoadManifest;
}
   
void LoadSceneView::onViewRemoved(ove::ViewStack& stateController)
{
}

void LoadSceneView::onViewStartFrame(ove::ViewStack& stateController)
{
}

void LoadSceneView::simulateView(ove::ViewStack& stateController, double dt)
{
}

void LoadSceneView::frameUpdateView
(
    ove::ViewStack& stateController,
    double dt,
    const cinek::uicore::InputState& inputState
)
{
    if (_currentTask != _nextTask) {
    
        _currentTask = _nextTask;
        switch (_currentTask) {
            case kLoadManifest:
                assetService().loadManifest("scenes/apartment.json",
                    [this](std::shared_ptr<ove::AssetManifest> manifest) {
                        _manifest = std::move(manifest);
                        _nextTask = kLoadScene;
                    });
                break;
                
            case kLoadScene:
                sceneService().load(_manifest,
                    [this](bool success) {
                        _nextTask = success ? kLoadPaths : kLoadError;
                    });
                break;
            case kLoadPaths:
                //  generates pathfinding data from current scene
                pathfinderService().generate(
                    [this](bool success) {
                        _nextTask = success ? kLoadSuccess : kLoadError;
                    });
                break;
            case kLoadError:
                break;
            case kLoadSuccess:
                stateController.present("GameView");
                break;
            default:
                break;
        }
        
    }
}

void LoadSceneView::onViewEndFrame(ove::ViewStack& stateController)
{
}

const char* LoadSceneView::viewId() const
{
    return "LoadSceneView";
}
    
}   /* namespace cinek */