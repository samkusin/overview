//
//  SceneJsonLoader.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/4/15.
//
//

#include "Scenes/Scene.hpp"
#include "SceneJsonLoader.hpp"
#include "Debug.hpp"
#include "EntityDatabase.hpp"
#include "Render/RenderGraph.hpp"

#include "Scenes/SceneDataContext.hpp"
#include "Scenes/SceneObjectJsonLoader.hpp"

#include "CKGfx/ModelJsonSerializer.hpp"

#include <ckjson/json.hpp>

namespace cinek {
    namespace ove {
    
    
struct SceneJsonLoader::Node
{
    enum Type
    {
        kNone,
        kGfxNode,
        kSceneTriMeshShape,
        kSceneObject,
    };
    Type type;
    gfx::NodeHandle gfxNodeHandle;
    union
    {
        btBvhTriangleMeshShape* sceneTriMeshShape;
    };
};
    
SceneJsonLoader::SceneJsonLoader
(
    SceneDataContext& context,
    gfx::Context& gfxContext,
    EntityDatabase& entityDb
) :
    _sceneContext(&context),
    _gfxContext(&gfxContext),
    _entityDb(&entityDb)
{
}

SceneJsonLoader::~SceneJsonLoader() = default;
    
Scene& SceneJsonLoader::operator()
(
    Scene& scene,
    RenderGraph& renderGraph,
    const JsonValue& jsonRoot
)
{
    JsonValue::ConstMemberIterator jsonNodesIt = jsonRoot.FindMember("nodes");
    if (jsonNodesIt == jsonRoot.MemberEnd())
        return scene;

    gfx::NodeJsonLoader gfxJsonLoader;
    gfxJsonLoader.context = _gfxContext;
    gfxJsonLoader.jsonMeshes = jsonRoot.FindMember("meshes");
    gfxJsonLoader.jsonLights = jsonRoot.FindMember("lights");
    gfxJsonLoader.jsonMaterials = jsonRoot.FindMember("materials");
    gfxJsonLoader.jsonAnimations = jsonRoot.FindMember("animations");
    
    SceneObjectJsonLoader sceneObjJsonLoader;
    sceneObjJsonLoader.context = _sceneContext;
    sceneObjJsonLoader.jsonHullsArrayIt = jsonRoot.FindMember("hulls");
    if (!sceneObjJsonLoader.jsonHullsArrayIt->value.Empty()) {
        sceneObjJsonLoader.hulls.reserve(sceneObjJsonLoader.jsonHullsArrayIt->value.Size());
    }
    
    const JsonValue& jsonNode = jsonNodesIt->value;
    
    Context context;
    context.scene = &scene;
    context.renderGraph = &renderGraph;
    context.gfxJsonLoader = &gfxJsonLoader;
    context.sceneObjectJsonLoader = &sceneObjJsonLoader;
    context.entity = 0;
    
    Node root;
    root.type = Node::kNone;
    
    Node node = parseJsonNode(context, root, jsonNode);
    context.renderGraph->nodeGraph().setRoot(node.gfxNodeHandle);
    
    return scene;
}

auto SceneJsonLoader::parseJsonNode
(
    Context context,
    const Node& parent,
    const JsonValue& jsonNode
)
-> Node
{
    Node node;
    const char* type = jsonNode["type"].GetString();
    
    //  Generate node depending on type and elements within node.
    //  Some scene nodes aren't added to the RenderGraph.  Instead they might
    //  contain component or other scene data.  Handle these special cases first.
    if (!strcasecmp(type, "hull")) {
        node.type = Node::kSceneTriMeshShape;
        node.sceneTriMeshShape = (*context.sceneObjectJsonLoader)(jsonNode);
    }
    else {
        if (jsonNode.HasMember("hulls")) {
            node.type = Node::kSceneObject;
        }
        else {
            node.type = Node::kGfxNode;
        }
        
        node.gfxNodeHandle = (*context.gfxJsonLoader)(context.renderGraph->nodeGraph(),
                                                      jsonNode);
        
        if (node.type == Node::kSceneObject) {
            node.sceneTriMeshShape = (*context.sceneObjectJsonLoader)(jsonNode);
        }
    }
    
    //  identify whether this node represents a new entity
    //  following the child node traversal, which accumulates component data,
    //  we can add components.
    bool newEntity = false;
    
    if (!context.entity) {
        if (!strcasecmp(type, "entity")) {
            //  note, this does not deal with 'child entities'.  we'll have to
            //  deal with this use-case when it comes up.
            context.entity = _entityDb->getStore(0).create();
        }
        //  render-based entities, (lights)
        else if (node.type == Node::kGfxNode) {
            auto gfxType = node.gfxNodeHandle->elementType();
            if (gfxType == gfx::Node::kElementTypeLight) {
                context.entity = _entityDb->getStore(0).create();
            }
        }

        newEntity = context.entity != 0;
    }
    
    //  traverse children
    
    //  either use the existing shape for entity bodies, or use a child
    //  version (supporting older scene formats.)
    btBvhTriangleMeshShape* triMeshShape = nullptr;
    if (node.type == Node::kSceneObject || node.type == Node::kSceneTriMeshShape)
        triMeshShape = node.sceneTriMeshShape;
    
    if (node.type == Node::kGfxNode && jsonNode.HasMember("children")) {
        auto& jsonChildren = jsonNode["children"];
        
        for (auto jsonChildIt = jsonChildren.Begin();
             jsonChildIt != jsonChildren.End();
             ++jsonChildIt)
        {
            Node child = parseJsonNode(context, node, *jsonChildIt);
            if (child.type == Node::kGfxNode) {
                context.renderGraph->nodeGraph().addChildNodeToNode(child.gfxNodeHandle, node.gfxNodeHandle);
            }
            else if (child.type == Node::kSceneTriMeshShape) {
                if (triMeshShape) {
                    _sceneContext->freeShape(triMeshShape);
                }
                triMeshShape = child.sceneTriMeshShape;
            }
        }
    }
    
    if (newEntity) {
        CK_ASSERT(context.entity);
        if (node.type == Node::kGfxNode) {
            context.renderGraph->setNodeEntity(context.entity, node.gfxNodeHandle);
        }
        //  support newer scene formats, or older ones where triMeshShapes
        //  were children of the entity gfx node.
        if (node.type == Node::kSceneObject ||
            (node.type == Node::kGfxNode && triMeshShape)) {
            
            SceneDataContext::SceneBodyInitParams initParams;
    
            initParams.collisionShape = triMeshShape;
            SceneBody* body = _sceneContext->allocateBody(initParams,
                node.gfxNodeHandle, context.entity);

            context.scene->attachBody(body);
        }
    }
        
    return node;
}
    
    } /* namespace ove */
} /* namespace cinek */
