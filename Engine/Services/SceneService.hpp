//
//  SceneService.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/02/15.
//
//

#ifndef Overview_Services_Scene_hpp
#define Overview_Services_Scene_hpp

#include "Engine/Scenes/SceneTypes.hpp"
#include "Engine/Messages/Scene.hpp"
#include "CKGfx/GfxTypes.hpp"

#include <functional>
#include <string>

namespace cinek { namespace ove {

class RenderService;

struct SceneServiceContext
{
    gfx::Context* gfxContext;
    Scene* scene;
    SceneDataContext* sceneData;
    SceneDebugDrawer* debugDrawer;
    MessageClientSender* sender;
    EntityDatabase* entityDb;
    RenderGraph* renderGraph;
};



class SceneService
{
public:    
    SceneService(const SceneServiceContext& context);
    
    void initialize(std::shared_ptr<AssetManifest> manifest);
    
    gfx::NodeHandle getGfxRootNode() const;
    
    SceneRayTestResult rayTestClosest(const btVector3& origin, const btVector3& dir,
                                      btScalar dist) const;
    
    
    //  debug methods
    void renderDebugStart(RenderService &renderService, const gfx::Camera& camera);
    void renderDebugAddRayTestHit(const SceneRayTestResult& rayTestResult,
        const btVector3& origin,
        btScalar radius,
        bool drawRay);
    void renderDebugAddLine(const btVector3& p0, const btVector3& p1,
        const gfx::Color3& color);
    void renderDebugEnd();
    
private:
    SceneServiceContext _context;
};

} /* namespace ove */ } /* namespace cinek */

#endif /* Overview_Services_Entity_hpp */
