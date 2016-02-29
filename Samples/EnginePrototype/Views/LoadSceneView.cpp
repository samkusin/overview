//
//  LoadSceneView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/8/16.
//
//

#include "LoadSceneView.hpp"
#include "Engine/Services/AssetService.hpp"

#include "Engine/Tasks/InitializeScene.hpp"
#include "Engine/Physics/Scene.hpp"
#include "Engine/Nav/Pathfinder.hpp"
#include "Engine/ViewStack.hpp"

#include <cinek/taskscheduler.hpp>

namespace cinek {

LoadSceneView::LoadSceneView(ApplicationContext* context) :
    AppViewController(context),
    _currentTask(kLoadStart),
    _nextTask(kLoadStart),
    _loader(context->sceneData,
            context->gfxContext,
            context->renderGraph,
            context->entityDatabase)
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
                
            case kLoadScene: {
                scheduler().schedule(ove::InitializeScene::create(_manifest, _loader,
                    [this](Task::State endState, Task& thisTask, void* ) {
                        if (endState == Task::State::kEnded) {
                            auto bodies = reinterpret_cast<ove::InitializeScene&>(thisTask).acquireBodyList();
                            
                            for (auto& body : bodies) {
                                scene().attachBody(body, body->categoryMask);
                            }
                            _nextTask = kLoadPaths;
                        }
                        else {
                            _nextTask = kLoadError;
                        }
                    })
                );
                }
                break;
            case kLoadPaths:
                //  generates pathfinding data from current scene
                pathfinder().generateFromScene(scene(), 
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