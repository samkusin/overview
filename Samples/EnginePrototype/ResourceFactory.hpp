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

#include <cinek/managed_dictionary.hpp>

namespace cinek {
    class TaskScheduler;
}

namespace cinek {
    namespace ove {
    
struct ResourceFactory : public AssetManfiestFactory
{
    ResourceFactory
    (
        gfx::Context& gfxContext,
        TaskScheduler& scheduler
    );

    //  AssetManifestFactory
    virtual RequestId onAssetManifestRequest(
        AssetType assetType,
        const std::string& name,
        std::function<void(const std::string&, LoadResult)> cb) override;
    
    virtual void onAssetManifestRequestCancelled(RequestId reqId) override;

private:
    gfx::Context* _gfxContext;
    TaskScheduler *_scheduler;
};
    
    }   /* namespace ove */
}   /* namespace cinek */


#endif /* ResourceStore_hpp */
