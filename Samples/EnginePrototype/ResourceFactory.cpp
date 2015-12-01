//
//  ResourceFactory.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/27/15.
//
//

#include "ResourceFactory.hpp"

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
}
    
auto ResourceFactory::onAssetManifestRequest
(
    AssetType assetType,
    const std::string& name,
    std::function<void(const std::string&, LoadResult)> cb
) -> RequestId
{
    switch (assetType)
    {
    case AssetType::kTexture:
        break;
    case AssetType::kModel:
        break;
    case AssetType::kNone:
        break;
    };
    return 0;
}
    
void ResourceFactory::onAssetManifestRequestCancelled(RequestId reqId)
{
}
    
    }   /* namespace ove */
}   /* namespace cinek */
