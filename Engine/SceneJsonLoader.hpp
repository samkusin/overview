//
//  SceneJsonLoader.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/4/15.
//
//

#ifndef Overview_SceneJsonLoader_hpp
#define Overview_SceneJsonLoader_hpp

#include "Engine/EngineTypes.hpp"
#include "CKGfx/GfxTypes.hpp"

#include <ckjson/jsontypes.hpp>
#include <vector>

namespace cinek {
    namespace ove {
    
/**
 *  Scene Loading
 *
 *  Traverses the Scene tree, constructing entities depending on node type.
 *  Upon finding an 'Entity' node, dispatch control to a separate tree traversal
 *  task.
 *
 */
    
class SceneJsonLoader
{
public:
    SceneJsonLoader(SceneDataContext* context,
                    gfx::Context* gfxContext,
                    RenderGraph* renderGraph,
                    EntityDatabase* entityDb);
    ~SceneJsonLoader();
    
    std::vector<SceneBody*> operator()(const JsonValue& jsonRoot);
    
private:
    struct Node;
    struct Context
    {
        std::vector<SceneBody*>* bodyList;
        gfx::NodeJsonLoader* gfxJsonLoader;
        SceneObjectJsonLoader* sceneObjectJsonLoader;
        Entity entity;
    };

    Node parseJsonNode(Context context, const Node& parent, const JsonValue& jsonNode);

    SceneDataContext* _sceneContext;
    gfx::Context* _gfxContext;
    RenderGraph* _renderGraph;
    EntityDatabase* _entityDb;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_SceneJsonLoader_hpp */
