//
//  AssetManifestFactory.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/27/15.
//
//

#ifndef Overview_AssetManifestFactory_hpp
#define Overview_AssetManifestFactory_hpp

#include "EngineTypes.hpp"

#include <string>
#include <functional>

namespace cinek {
    namespace ove {
    
/**
 *  @class  AssetManifestFactory
 *  @brief  Responsible for loading and maintaining assets referenced in an
 *          AssetManifest.
 */
class AssetManfiestFactory
{
public:
    enum class LoadResult
    {
        kSuccess,
        kFailed,
        kAborted
    };
    
    using RequestId = uint32_t;
    using RequestCb = std::function<void(const std::string&, LoadResult)>;
    
    virtual ~AssetManfiestFactory() {}
    
    virtual RequestId onAssetManifestRequest(
        AssetType assetType,
        const std::string& name,
        RequestCb cb) = 0;
    
    virtual void onAssetManifestRequestCancelled(RequestId reqId) = 0;
};

    } /* namespace ove */
} /* namespace cinek */


#endif /* Overview_AssetManifestFactory_hpp */
