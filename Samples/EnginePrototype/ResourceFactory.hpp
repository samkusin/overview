//
//  ResourceFactory.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/27/15.
//
//

#ifndef Overview_ResourceFactory_hpp
#define Overview_ResourceFactory_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/AssetManifestFactory.hpp"
#include "CKGfx/Context.hpp"

#include <cinek/task.hpp>
#include <vector>

namespace cinek {
    class TaskScheduler;
}

namespace cinek {
    namespace ove {
    
struct ResourceFactory : public AssetManfiestFactory
{
    ResourceFactory
    (
        gfx::Context* gfxContext,
        TaskScheduler* scheduler
    );
    
    virtual ~ResourceFactory();

    //  AssetManifestFactory
    virtual RequestId onAssetManifestRequest(
        AssetType assetType,
        const std::string& name,
        RequestCb cb) override;
    
    virtual void onAssetManifestRequestCancelled(RequestId reqId) override;

private:
    void requestFinished(TaskId taskId, const std::string& name,
                         Task::State state);
private:
    gfx::Context* _gfxContext;
    TaskScheduler *_scheduler;
    
    std::vector<std::pair<TaskId, RequestCb>> _requests;
};
    
    }   /* namespace ove */
}   /* namespace cinek */


#endif /* ResourceStore_hpp */
