//
//  LoadAssets.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/29/15.
//
//

#ifndef Overview_Task_LoadAssets_hpp
#define Overview_Task_LoadAssets_hpp

#include "Engine/AssetManifest.hpp"
#include "Engine/AssetManifestLoader.hpp"

#include <cinek/task.hpp>

namespace cinek {
    namespace ove {

class LoadAssets : public Task
{
public:
    static const UUID kUUID;
    
    LoadAssets(EndCallback cb=0);
    
    void setManifest(unique_ptr<AssetManifest>&& manifest, AssetManfiestFactory& factory) {
        _manifest = std::move(manifest);
        _factory = &factory;
        _loader = std::move(AssetManifestLoader(*_manifest, *_factory));
    }
    
    unique_ptr<AssetManifest> acquireManifest();
    
    virtual const TaskClassId& classId() const override { return kUUID; }
    
private:
    virtual void onBegin() override;
    virtual void onUpdate(uint32_t deltaTimeMs) override;
    virtual void onCancel() override;
    
private:    
    unique_ptr<AssetManifest> _manifest;
    AssetManfiestFactory* _factory;
    AssetManifestLoader _loader;
};
    
    }  /* namespace ove */
}  /* namespace cinek */


#endif /* Overview_Task_LoadAssets_hpp */
