//
//  ResourceFactory.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/27/15.
//
//

#include "ResourceFactory.hpp"

#include "Engine/Tasks/LoadTextureAsset.hpp"
#include "Engine/Tasks/LoadAssetManifest.hpp"

#include "CKGfx/ModelJsonSerializer.hpp"

#include <cinek/taskscheduler.hpp>

namespace cinek {
    namespace ove {
    
ResourceFactory::ResourceFactory
(
    gfx::Context& context,
    TaskScheduler& scheduler
) :
    _gfxContext(&context),
    _scheduler(&scheduler)
{
    _requests.reserve(4);
}

ResourceFactory::~ResourceFactory()
{
    for (auto& req : _requests) {
        _scheduler->cancel(req.first);
    }
}
    
auto ResourceFactory::onAssetManifestRequest
(
    AssetType assetType,
    const std::string& name,
    std::function<void(const std::string&, LoadResult)> cb
) -> RequestId
{
    RequestId reqId = 0;
    switch (assetType)
    {
    case AssetType::kTexture:
        if (!_gfxContext->findTexture(name.c_str())) {
            reqId = _scheduler->schedule(allocate_unique<LoadTextureAsset>(
                name,
                [this](Task::State state, Task& t) {
                    auto& task = static_cast<LoadTextureAsset&>(t);
                    if (state == Task::State::kEnded) {
                        _gfxContext->registerTexture(task.acquireTexture(), task.name().c_str());
                    }
                    requestFinished(t.id(), task.name(), state);
                })
            );
        }
        break;
    case AssetType::kModel:
        if (!_gfxContext->findModel(name.c_str())) {
            reqId = _scheduler->schedule(allocate_unique<LoadAssetManifest>(
                name,
                *this,
                [this](Task::State state, Task& t) {
                    auto& task = static_cast<LoadAssetManifest&>(t);
                    if (state == Task::State::kEnded) {
                        auto manifest = task.acquireManifest();
                        auto model = gfx::loadNodeGraphFromJSON(*_gfxContext, manifest->root());
                        _gfxContext->registerModel(std::move(model), task.name().c_str());
                    }
                    requestFinished(t.id(), task.name(), state);
                })
            );
        }
        break;
    case AssetType::kNone:
        break;
    };
    return reqId;
}

void ResourceFactory::requestFinished
(
    TaskId taskId,
    const std::string& name,
    Task::State state
)
{
    auto it = std::find_if(_requests.begin(), _requests.end(),
        [taskId](const decltype(_requests)::value_type& a) -> bool {
            return (a.first == taskId);
        });
    if (it == _requests.end())
        return;
    
    LoadResult res = LoadResult::kFailed;
    if (state == Task::State::kEnded)
        res = LoadResult::kSuccess;

    it->second(name, res);
    
    _requests.erase(it);
}
    
void ResourceFactory::onAssetManifestRequestCancelled(RequestId reqId)
{
    auto it = std::find_if(_requests.begin(), _requests.end(),
        [reqId](const decltype(_requests)::value_type& a) -> bool {
            return (a.first == reqId);
        });
    if (it == _requests.end())
        return;
    
    _scheduler->cancel(it->first);
    _requests.erase(it);
}
    
    }   /* namespace ove */
}   /* namespace cinek */
