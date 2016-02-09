//
//  SceneService.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/02/15.
//
//

#ifndef Overview_Services_Scene_hpp
#define Overview_Services_Scene_hpp

#include "Engine/Physics/SceneTypes.hpp"
#include "Engine/Messages/Scene.hpp"
#include "CKGfx/GfxTypes.hpp"

#include <functional>
#include <string>

namespace cinek {

class TaskScheduler;

namespace ove {

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
    TaskScheduler* taskScheduler;
};



class SceneService
{
public:    
    SceneService(const SceneServiceContext& context);
    ~SceneService();
    
    void load(std::shared_ptr<AssetManifest> manifest, std::function<void(bool)> cb);
    
    gfx::NodeHandle getGfxRootNode() const;
    
    SceneRayTestResult rayTestClosest
    (
        const btVector3& origin,
        const btVector3& dir,
        btScalar dist
    )
    const;
    
    //  useful for switching between "running" and "editor" modes
    void disableSimulation();
    void enableSimulation();
    bool isSimulationRunning() const;
    
    //  modify entity properties
    void setEntityPosition
    (
        Entity entity,
        const btVector3& pos,
        btVector3 up
    );
    
    //  debug methods
    void renderDebugStart(RenderService &renderService, const gfx::Camera& camera);
    void renderDebugAddRayTestHit
    (
        const SceneRayTestResult& rayTestResult,
        const btVector3& origin,
        btScalar radius,
        bool drawRay
    );
    void renderDebugAddLine
    (
        const btVector3& p0,
        const btVector3& p1,
        const gfx::Color3& color
    );
    void renderDebugEnd();
    
private:
    SceneServiceContext _context;
};

} /* namespace ove */ } /* namespace cinek */

#endif /* Overview_Services_Entity_hpp */
