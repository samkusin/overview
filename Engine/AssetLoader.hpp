//
//  AssetLoader.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/24/15.
//
//

#ifndef Overview_AssetLoader_hpp
#define Overview_AssetLoader_hpp

#include "EngineTypes.hpp"
#include "Asset.hpp"

#include <cinek/allocator.hpp>
#include <ckio/file.h>

namespace cinek {
    namespace ove {
  
/**
 *  @class  AssetLoader
 *  @brief  Loads asset objects
 *  
 *  The AssetLoader is a Factory for Asset objects.  Assets are usually an
 *  intermediate object between file and its target system.  For details,
 *  consult the documentation on asset objects.
 */
class AssetLoader
{
    CK_CLASS_NON_COPYABLE(AssetLoader);
    
public:
    AssetLoader();
    ~AssetLoader();
    
    enum class Result
    {
        kStarted,
        kInProgress,
        kFailed,
        kAborted,
        kOutOfMemory,
        kSuccess,
        kIdle
    };
    
    Result load(const char* pathname, std::function<void(Result, unique_ptr<Asset>)> fn);
    Result status() const;
    
    void update();
    
private:
    std::function<void(Result, unique_ptr<Asset>)> _callback;
    ckio_handle* _activeCKIO;
    Result _activeResult;
    unique_ptr<Asset> _activeAsset;
};
    
    }   /* namespace ove */
}   /* namespace cinek */

#endif /* Overview_AssetLoader_hpp */
