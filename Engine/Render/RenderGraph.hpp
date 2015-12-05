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
     *  Dereferences the gfx Node associated with the supplied entity.  Removing
     *  is a O(log n) operation in most cases.  It is a O(n) operation if 
     *  removing nodes that have been added before calling prepare(), which
     *  should be an edge case.
     *  
     *  @param  e   The Entity to remove
     */
    void removeNode(Entity e);
    /**
     *  Clears the render graph to entity map.
     */
    void clear();
    /**
     *  @return The root of the generated gfx::NodeGraph.
     */
    gfx::NodeHandle root() const;
    
    /**
     *  @return The underlying node graph
     */
    cinek::gfx::NodeGraph& nodeGraph() {
        return _nodeGraph;
    }

private:
    void updateRenderNodes(double dt);

private:
    cinek::gfx::NodeGraph _nodeGraph;
    double _renderTime;
    
    struct Node
    {
        Entity entity;
        gfx::NodeHandle gfxNode;
        void* context;
    };
    
    //  nodes ordered by Entity
    std::vector<Node> _renderNodes;
    //
    //  index of last active & pending destroyed nodes in the nodelist
    //  new nodes are placed after _nodeEndIndex to the end of the rendernode
    //  list.  this is an optimization so that we can differentiate new vs
    //  active nodes in the list - and allows us to sort the list once per frame
    uint32_t _nodeEndIndex;
    //  used to trigger resort of node vector during prepare
    uint32_t _removedNodeCount;
    //  smaller values mean that it takes more removed entities to trigger
    //  a resort
    static constexpr uint32_t kRemoveNodeSortFactor = 10;
    
    //  Animations
    //
    gfx::AnimationControllerPool _animControllerPool;
    
    struct AnimNode
    {
        Entity entity;
        gfx::AnimationControllerHandle animController;
    };
    std::vector<AnimNode> _animNodes;
};
    
    }   /* namesapce ove */
}   /* namespace cinek */

#endif /* Overview_RenderGraph_hpp */
