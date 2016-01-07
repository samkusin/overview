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
    RenderService(const RenderContext& context);
    
    const gfx::Rect& getViewRect() const { return _context.frameRect; }
    
    void renderNodeWithCamera(gfx::NodeHandle node, const gfx::Camera& camera);
    
private:
    friend class SceneService;
    
    const RenderContext& renderContext() const { return _context; }
    
    RenderContext _context;
    Scene* _scene;
    SceneDebugDrawer* _sceneDbgDrawer;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* RenderService_hpp */
