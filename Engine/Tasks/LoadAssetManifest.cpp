//
//  LoadAssetManifest.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/29/15.
//
//

#include "LoadAssetManifest.hpp"

#include "Engine/Debug.hpp"

namespace cinek {
    namespace ove {

const UUID LoadAssetManifest::kUUID = {
    0x71, 0x47, 0xd6, 0x65, 0xfa, 0x62, 0x4e, 0xf5,
    0xa1, 0x0c, 0xd7, 0xf2, 0xc6, 0x3c, 0xe0, 0x34
};
        

LoadAssetManifest::LoadAssetManifest
(
    std::string name,
    AssetManfiestFactory& factory,
    EndCallback cb
) :
    LoadFile(name, cb),
    _factory(&factory)
{

}
    
std::shared_ptr<AssetManifest> LoadAssetManifest::acquireManifest()
{
    return std::move(_manifest);
}

void LoadAssetManifest::onFileLoaded()
{
    std::string manifestName = name();
    auto idx = manifestName.rfind('.');
    if (idx != std::string::npos) {
        manifestName.erase(idx);
    }
    _manifest = std::allocate_shared<AssetManifest>(
                    std_allocator<AssetManifest>(),
                    std::move(manifestName),
                    std::move(acquireBuffer())
                );
    
    _loader = std::move(AssetManifestLoader(*_manifest.get(), *_factory));
    _loader.start([this](AssetManifestLoader::LoadResult r) {
        end();
    });
}

void LoadAssetManifest::onUpdate(uint32_t deltaTimeMs)
{
    LoadFile::onUpdate(deltaTimeMs);

    if (_manifest) {
        _loader.update();
    }
}

void LoadAssetManifest::onCancel()
{
    if (_manifest) {
        _loader.cancel();
    }

    LoadFile::onCancel();
}
 
    }  /* namespace ove */
}  /* namespace cinek */
