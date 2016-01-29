//
//  RenderGraph.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/20/15.
//
//

#ifndef Overview_RenderGraph_hpp
#define Overview_RenderGraph_hpp

#include "Engine/EngineTypes.hpp"
#include "CKGfx/NodeGraph.hpp"

#include <vector>

namespace cinek {
    namespace ove {

/**
 *  @class  RenderGraph
 *  @brief  Manages a scene graph
 *
 *
 */
class RenderGraph
{
public:
    RenderGraph(const gfx::NodeElementCounts& counts, uint32_t entityCount,
                uint32_t animCount);
    
    /**
     *  Systems flag an entity for rendering with a given source graphics node.
     *  RenderGraph will clone the node.  To apply any custom preparation during
     *  the prepare stage, systems can supply a delegate.  This delegate
     *  is invoked when calling prepare()
     *
     *  Adding is a O(1) operation.  This is possible since internally this
     *  item is sorted into the node vector during prepare().
     *
     *  @param  e           The entity (acts as a key) 
     *  @param  sourceNode  The graphics node to clone into the scene graph
     *  @param  context     A context pointer associated with the Node (used
     *                      during prepare() when the preparation delegate is
     *                      called.)
     *  @return A handle to the created Node within the NodeGraph
     */
    gfx::NodeHandle cloneAndAddNode(Entity e, gfx::NodeHandle sourceNode,
                                    void* context);
    /**
     *  @param  e           The entity (acts as a key)
     *  @param  h           The node handle (created from this RenderGraph's
     *                      node graph.)
     */
    gfx::NodeHandle setNodeEntity(Entity e, gfx::NodeHandle h);
    /**
     *  Dereferences the gfx Node associated with the supplied entity.  Removing
     *  is a O(log n) operation in most cases.  It is a O(n) operation if 
     *  removing nodes that have been added before calling prepare(), which
     *  should be an edge case.
     *  
     *  @param  e   The Entity to remove
     */
    void removeNode(Entity e);
    /**
     *  Find the node associated with an entity.
     *
     *  @param  entity  The entity to lookup
     *  @return The associated node, or an empty node if there is no entity
     *          mapped to a node.
     */
    gfx::NodeHandle findNode(Entity e) const;
    /**
     *  Clears the render graph to entity map.
     */
    void clear();
    /**
     *  Updates the render graph
     */
    void update(double dt);
    /**
     *  @return The root of the generated gfx::NodeGraph.
     */
    gfx::NodeHandle root() const;
    /**
     *  @return The associated node graph
     */
    gfx::NodeGraph& nodeGraph() {
        return _nodeGraph;
    }

private:
    //  controller objects are referenced by the nodegraph and our own animation
    //  controller update pass
    gfx::AnimationControllerPool _animControllerPool;
    
    struct AnimNode
    {
        Entity entity;
        gfx::AnimationControllerHandle animController;
    };

    cinek::gfx::NodeGraph _nodeGraph;
    double _renderTime;

    struct Node
    {
        Entity entity;
        gfx::NodeHandle gfxNode;
        void* context;
    };
    
    //  pending node lists
    std::vector<Node> _pendingRenderNodes;
    std::vector<Entity> _removedRenderNodes;
    
    //  nodes ordered by Entity
    std::vector<Node> _renderNodes;
    std::vector<AnimNode> _animNodes;

    AnimNode* addAnimNode(Entity e, gfx::AnimationControllerHandle h);
};
    
    }   /* namesapce ove */
}   /* namespace cinek */

#endif /* Overview_RenderGraph_hpp */
