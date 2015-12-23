//
//  SceneService.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/02/15.
//
//

#ifndef Overview_Services_Scene_hpp
#define Overview_Services_Scene_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/Messages/Scene.hpp"
#include "CKGfx/GfxTypes.hpp"

#include <functional>
#include <string>

namespace cinek { namespace ove {

struct SceneServiceContext
{
    gfx::Context* gfxContext;
    Scene* scene;
    SceneDataContext* sceneData;
    MessageClientSender* sender;
    EntityDatabase* entityDb;
    RenderGraph* renderGraph;
};

class SceneService
{
public:    
    SceneService(const SceneServiceContext& context);
    
    void initializeScene(std::shared_ptr<AssetManifest> manifest);

private:
    SceneServiceContext _context;
};

} /* namespace ove */ } /* namespace cinek */

#endif /* Overview_Services_Entity_hpp */
