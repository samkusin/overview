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

void RenderService::renderNodeToTarget
(
    gfx::NodeRenderer& renderer,
    const gfx::RenderTarget& target,
    gfx::NodeHandle node,
    const gfx::Camera& camera
)
{
    renderer(*_context->programs, *_context->uniforms, target, camera, node);
}

gfx::LightHandle RenderService::createLight(gfx::Light light)
{
    return _gfxContext->registerLight(std::move(light));
}

gfx::ModelSetHandle RenderService::findModelSet(const char *name) const
{
    return _gfxContext->findModelSet(name);
}

gfx::TextureHandle RenderService::findTexture(const char* name) const
{
    return _gfxContext->findTexture(name);
}

bgfx::UniformHandle RenderService::bgfxUniformHandle(gfx::NodeUniformSlot uniformSlot) const
{
    return _context->uniforms->at(uniformSlot);
}

bgfx::ProgramHandle RenderService::bgfxProgramHandle(gfx::NodeProgramSlot programSlot) const
{
    return _context->programs->at(programSlot);
}
    
    } /* namespace ove */
} /* namespace cinek */