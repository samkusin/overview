//
//  LoadAssets.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/29/15.
//
//

#include "LoadAssets.hpp"

namespace cinek {
    namespace ove {
 
const UUID LoadAssets::kUUID = {
    0x30, 0x66, 0x1b, 0x4c, 0x6f, 0x18, 0x42, 0x62,
    0xb6, 0xea, 0x9c, 0xd2, 0xa6, 0x0a, 0xaf, 0x3e
};


LoadAssets::LoadAssets
(
    EndCallback cb
) :
    Task(cb)
{

}

unique_ptr<AssetManifest> LoadAssets::acquireManifest()
{
    return std::move(_manifest);
}

void LoadAssets::onBegin()
{
    _loader.start([this](AssetManifestLoader::LoadResult r) {
        end();
    });
}

void LoadAssets::onUpdate(uint32_t deltaTimeMs)
{
    _loader.update();
}

void LoadAssets::onCancel()
{
    _loader.cancel();
}
    
    }  /* namespace ove */
}  /* namespace cinek */
