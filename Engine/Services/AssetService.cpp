//
//  AssetService.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/22/15.
//
//

#include "AssetService.hpp"

#include "Engine/Tasks/LoadAssetManifest.hpp"

#include <cinek/taskscheduler.hpp>

namespace cinek {
    namespace ove {
    
AssetService::AssetService(const AssetServiceContext& context) :
    _context(context)
{

}

AssetService::~AssetService()
{
    _context.taskScheduler->cancelAll(this);
}

    
void AssetService::loadManifest
(
    const std::string& name,
    std::function<void(std::shared_ptr<AssetManifest>)> resultCb
)
{
    auto taskCb = [resultCb](Task::State state, Task& task) {
        if (state == Task::State::kEnded) {
            auto manifest = reinterpret_cast<LoadAssetManifest&>(task).acquireManifest();
            resultCb(manifest);
        }
        else if (state == Task::State::kFailed) {
            resultCb(nullptr);
        }
    };

    auto task = allocate_unique<cinek::ove::LoadAssetManifest>(
        name,
        *_context.resourceFactory,
        taskCb
    );
    
    _context.taskScheduler->schedule(std::move(task), this);
}
    
    } /* namespace ove */
} /* namespace cinek */