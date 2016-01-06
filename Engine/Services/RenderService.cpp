//
//  RenderService.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/4/16.
//
//

#include "RenderService.hpp"

#include "CKGfx/Camera.hpp"

namespace cinek {
    namespace ove {

RenderService::RenderService
(
    const RenderContext& context
) :
    _context(context)
{
}


void RenderService::renderNodeWithCamera
(
    gfx::NodeHandle node,
    const gfx::Camera& camera
)
{
    (*_context.renderer)(*_context.programs, *_context.uniforms, camera, node);
}


    
    } /* namespace ove */
} /* namespace cinek */