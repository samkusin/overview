//
//  LoadAssetManifest.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/29/15.
//
//

#ifndef Overview_Task_LoadAssetManifest_hpp
#define Overview_Task_LoadAssetManifest_hpp

#include "Engine/AssetManifest.hpp"
#include "Engine/AssetManifestLoader.hpp"
#include "Engine/Tasks/LoadFile.hpp"

#include <cinek/allocator.hpp>

#include <vector>

namespace cinek {
    namespace ove {

class LoadAssetManifest : public LoadFile
{
public:
    static const UUID kUUID;
    
    LoadAssetManifest(EndCallback cb=0) : LoadFile(cb), _factory(nullptr) {}
    LoadAssetManifest(std::string name, AssetManfiestFactory& factory,
                      EndCallback cb=0);

    unique_ptr<AssetManifest> acquireManifest();
    
    virtual const TaskClassId& classId() const override { return kUUID; }
    
private:
    virtual void onFileLoaded() override;
    
    virtual void onUpdate(uint32_t deltaTimeMs) override;
    virtual void onCancel() override;
        
private:
    unique_ptr<AssetManifest> _manifest;
    AssetManfiestFactory* _factory;
    AssetManifestLoader _loader;
};
    
    }  /* namespace ove */
}  /* namespace cinek */


#endif /* Overview_Task_LoadAssetManifest_hpp */
