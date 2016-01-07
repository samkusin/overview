//
//  RenderContext.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/4/16.
//
//

#ifndef Overview_RenderContext_hpp
#define Overview_RenderContext_hpp

#include "Engine/EngineTypes.hpp"
#include "CKGfx/GfxTypes.hpp"
#include "CKGfx/NodeRenderer.hpp"

namespace cinek {
    namespace ove {
    
    struct RenderContext
    {
        gfx::Rect frameRect;
        gfx::NodeRenderer::ProgramMap* programs;
        gfx::NodeRenderer::UniformMap* uniforms;
    };

    }
}

#endif /* Overview_RenderContext_hpp */
