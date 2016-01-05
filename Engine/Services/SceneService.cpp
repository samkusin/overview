//
//  SceneService.inl
//  EnginePrototype
//
//  Created by Samir Sinha on 12/02/15.
//
//

#include "SceneService.hpp"
#include "RenderService.hpp"
#include "Engine/Scenes/Scene.hpp"
#include "Engine/Scenes/SceneDebugDrawer.hpp"
#include "Engine/Render/RenderGraph.hpp"
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

void SceneService::renderSceneDebug
(
    RenderService &renderService,
    const gfx::Camera &camera
)
{
    const RenderContext& renderContext = renderService.renderContext();
    
    _context.debugDrawer->setup(*renderContext.programs, *renderContext.uniforms, camera);
    _context.scene->debugRender();
}


gfx::NodeHandle SceneService::getGfxRootNode() const
{
    return _context.renderGraph->root();
}

    } /* namespace ove */
} /* namespace cinek */
