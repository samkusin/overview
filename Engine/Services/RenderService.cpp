//
//  RenderService.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/4/16.
//
//

#include "RenderService.hpp"

#include "CKGfx/Camera.hpp"
#include "CKGfx/Light.hpp"
#include "CKGfx/Context.hpp"

namespace cinek {
    namespace ove {

RenderService::RenderService
(
    gfx::Context* gfxContext,
    RenderContext* context
) :
    _gfxContext(gfxContext),
    _context(context)
{
}


void RenderService::renderNode
(
    gfx::NodeRenderer& renderer,
    gfx::NodeHandle node,
    const gfx::Camera& camera
)
{
    renderer(*_context->programs, *_context->uniforms, camera, node);
}

gfx::LightHandle RenderService::createLight(gfx::Light light)
{
    return _gfxContext->registerLight(std::move(light));
}
    
    } /* namespace ove */
} /* namespace cinek */