//
//  GameEntityFactory.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/10/15.
//
//

#ifndef Prototype_EntityFactory_hpp
#define Prototype_EntityFactory_hpp

#include "Engine/EntityDatabase.hpp"
#include "CKGfx/GfxTypes.hpp"

namespace cinek {

class GameEntityFactory : public ove::EntityComponentFactory
{

public:
    GameEntityFactory
    (
        gfx::Context& gfxContext,
        ove::SceneDataContext& sceneData,
        ove::Scene& scene,
        ove::RenderGraph& renderGraph
    );
                      
    virtual void onCustomComponentCreateFn(Entity entity,
                        EntityStore& store,
                        const std::string& componentName,
                        const cinek::JsonValue& definitions,
                        const cinek::JsonValue& compTemplate);
    
    virtual void onCustomComponentEntityDestroyFn(Entity entity);
                        
    virtual void onCustomComponentEntityCloneFn(Entity target, Entity origin);

private:
    gfx::Context* _gfxContext;
    ove::SceneDataContext* _sceneDataContext;
    ove::Scene* _scene;
    ove::RenderGraph* _renderGraph;
};

}

#endif /* EntityFactory_hpp */
