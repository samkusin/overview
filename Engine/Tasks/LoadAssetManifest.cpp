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
    EndCallback cb
) :
    LoadFile(name, cb)
{

}
    
unique_ptr<AssetManifest>  LoadAssetManifest::acquireManifest()
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
    _manifest = allocate_unique<AssetManifest>(
                    std::move(manifestName),
                    std::move(acquireBuffer())
                );
    
    LoadFile::onFileLoaded();
}
 
    }  /* namespace ove */
}  /* namespace cinek */
