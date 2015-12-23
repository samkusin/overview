//
//  SceneService.inl
//  EnginePrototype
//
//  Created by Samir Sinha on 12/02/15.
//
//

#include "SceneService.hpp"
#include "Engine/Scenes/Scene.hpp"
#include "Engine/SceneJsonLoader.hpp"
#include "Engine/AssetManifest.hpp"
#include "Engine/Debug.hpp"

#include <ckmsg/client.hpp>


namespace cinek {
    namespace ove {

SceneService::SceneService
(
    const SceneServiceContext& context
) :
    _context(context)
{
}


void SceneService::initializeScene(std::shared_ptr<AssetManifest> manifest)
{
    //  use shared pointer in case we need to persist the manifest (like we do for entity
    //  manifests.)

    SceneJsonLoader loader(*_context.sceneData,
                           *_context.gfxContext,
                           *_context.entityDb);
            
    loader(*_context.scene, *_context.renderGraph, manifest->root());
}


    } /* namespace ove */
} /* namespace cinek */
