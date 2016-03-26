//
//  GameEntityFactory.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/10/15.
//
//

#ifndef Prototype_EntityFactory_hpp
#define Prototype_EntityFactory_hpp

#include "GameTypes.hpp"

#include "Engine/EntityDatabase.hpp"

#include "CKGfx/GfxTypes.hpp"

namespace cinek {

class GameEntityFactory : public ove::EntityComponentFactory
{

public:
    GameEntityFactory
    (
        gfx::Context* gfxContext,
        ove::SceneDataContext* sceneData,
        ove::Scene* scene,
        ove::RenderGraph* renderGraph,
        NavDataContext* navDataContext,
        ove::NavSystem* navSystem,
        TransformDataContext* transformDataContext,
        ove::TransformSystem* transformSystem
    );
                      
    virtual void onCustomComponentCreateFn(Entity entity,
                        EntityStore& store,
                        const std::string& templateName,
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
    NavDataContext* _navDataContext;
    ove::NavSystem* _navSystem;
    TransformDataContext* _transformDataContext;
    ove::TransformSystem* _transformSystem;
};

}

#endif /* EntityFactory_hpp */
