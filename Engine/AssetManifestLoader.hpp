//
//  AssetManifestLoader.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/26/15.
//
//

#ifndef Overview_AssetManifestLoader_hpp
#define Overview_AssetManifestLoader_hpp

#include "AssetManifest.hpp"

#include <ckjson/json.hpp>
#include <vector>
#include <string>
#include <functional>

namespace cinek {
    namespace ove {

/**
 *  @class  AssetManifestLoader
 *  @brief  Controls the loading of assets from an AssetManifest.
 *
 *  AssetManifestLoader manages the loading of assets from an AssetManifest
 *  using an AssetManifestFactory.  Only a single manifest can be loaded at
 *  one time per instance.  But via the AssetManifestFactory, it's possible to
 *  spawn manifest loaders if necessary based on content from the parent
 *  manifest.
 */
class AssetManifestLoader
{
    CK_CLASS_NON_COPYABLE(AssetManifestLoader);
    
public:
    AssetManifestLoader();
    AssetManifestLoader(AssetManifest& manifest, AssetManfiestFactory& factory);
    
    AssetManifestLoader(AssetManifestLoader&& other);
    AssetManifestLoader& operator=(AssetManifestLoader&& other);
    
    enum class LoadResult
    {
        kSuccess,
        kFailed,
        kAborted
    };
    
    using LoadResultCb = std::function<void(LoadResult)>;
    
    void start(LoadResultCb resultCb);
    void cancel();
    void update();
    
private:
    void end(LoadResult result);
    
    void requestAssetLoad(AssetType assetType, const std::string& name);
    
private:
    AssetManfiestFactory* _listener;
    AssetManifest* _manifest;
    LoadResultCb _loadCb;

    struct State
    {
        enum { iter_size = sizeof_max<JsonValue::ConstMemberIterator,
                                      JsonValue::ConstValueIterator>::size };
        enum Class
        {
            kNone           = 0x0000,
            kMember         = 0x0001,
            kArray          = 0x0002
        };
        
        State();
        State(AssetType type,
              JsonValue::ConstMemberIterator iter,
              JsonValue::ConstMemberIterator iterEnd);
        State(AssetType classId,
              JsonValue::ConstValueIterator iter,
              JsonValue::ConstValueIterator iterEnd);
        ~State();
        
        Class classType() const { return _class; }
        AssetType assetType() const { return _assetType; }
        
        const JsonValue::Member* currentMember() const;
        const JsonValue* currentValue() const;
        
        State& increment();
        bool done() const;
        
    private:
        Class _class;
        AssetType _assetType;
        
        JsonValue::ConstMemberIterator& memberIterator();
        const JsonValue::ConstMemberIterator& memberIterator() const;
        const JsonValue::ConstMemberIterator& memberIteratorEnd() const;
        JsonValue::ConstValueIterator& iterator();
        const JsonValue::ConstValueIterator& iterator() const;
        const JsonValue::ConstValueIterator& iteratorEnd() const;
        
        uint8_t _iterBuf[iter_size];
        uint8_t _iterEndBuf[iter_size];
    };
    
    std::vector<State> _stack;
    int _requestId;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_AssetManifestLoader_hpp */
