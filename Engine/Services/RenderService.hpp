//
//  RenderService.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/4/16.
//
//

#ifndef Overview_RenderService_hpp
#define Overview_RenderService_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/Render/RenderContext.hpp"

namespace cinek {
    namespace ove {

class RenderService
{
public:
    RenderService(gfx::Context* gfxContext, RenderContext* context);
    
    const gfx::Rect& getViewRect() const { return _context->frameRect; }
    
    void renderNode(gfx::NodeRenderer& renderer,
        gfx::NodeHandle node,
        const gfx::Camera& camera);
    
    void renderNodeToTarget(gfx::NodeRenderer& renderer,
        const gfx::RenderTarget& target,
        gfx::NodeHandle node,
        const gfx::Camera& camera);
    
    bgfx::UniformHandle bgfxUniformHandle(gfx::NodeUniformSlot uniformSlot) const;
    bgfx::ProgramHandle bgfxProgramHandle(gfx::NodeProgramSlot programSlot) const;
    
    //  Finds a Model given its name
    gfx::ModelSetHandle findModelSet(const char* name) const;
    
    gfx::LightHandle createLight(gfx::Light light);
    
private:
    friend class SceneService;
    
    const RenderContext& renderContext() const { return *_context; }
    
    gfx::Context* _gfxContext;
    RenderContext* _context;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* RenderService_hpp */
