//
//  InitializeScene.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/8/16.
//
//

#include "InitializeScene.hpp"
#include "Engine/AssetManifest.hpp"

namespace cinek {
    namespace ove {
    
const UUID InitializeScene::kUUID = {
    0x65, 0xda, 0xf1, 0xee, 0x81, 0xb1, 0x43, 0xc7,
    0x94, 0x5f, 0x53, 0x25, 0xdb, 0xf7, 0xdc, 0x8c
};

unique_ptr<InitializeScene> InitializeScene::create
(
    std::shared_ptr<AssetManifest> inputManifest,
    SceneJsonLoader loader,
    EndCallback cb
)
{
    return allocate_unique<InitializeScene>(std::move(inputManifest),
                std::move(loader),
                std::move(cb));
}

InitializeScene::InitializeScene
(
    std::shared_ptr<AssetManifest> inputManifest,
    SceneJsonLoader loader,
    EndCallback cb
) :
    Task(cb),
    _manifest(std::move(inputManifest)),
    _loader(loader)
    
{
}

SceneJsonLoader::SceneBodyList InitializeScene::acquireBodyList()
{
    return std::move(_bodyList);
}

void InitializeScene::onUpdate(uint32_t deltaTimeMs)
{
    _bodyList = _loader(_manifest->root());
    end();
}

    
    }  /* namespace ove */
}  /* namespace cinek */