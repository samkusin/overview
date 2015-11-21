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
 *  RenderGraph requires a delegate type that conforms to the following
 *  concepts:
 *
 *  - void delegate(Entity entity,
 *                  _Vector& pos, _Basis& basis, float scalar);
 *  - operator bool()
 *  - support move
 */
template<typename _Vector, typename _Basis, typename _NodeDelegate>
class RenderGraph
{
public:
    RenderGraph(const gfx::NodeElementCounts& counts);
    
    /**
     *  Systems flag an entity for rendering with a given source graphics node.
     *  RenderGraph will clone the node.  To apply any custom preparation during
     *  the prepare stage, systems can supply a delegate.  This delegate
     *  is invoked when calling prepare()
     *
     *  @param  e           The entity (acts as a key) 
     *  @param  sourceNode  The graphics node to clone into the scene graph
     *  @param  prepareDel  The delegate invoked during prepare
     */
    void cloneAndAddNodeWithDelegate(Entity e, gfx::Node sourceNode,
                                    _NodeDelegate prepareDel=_NodeDelegate());
    /**
     *
     */
    void removeNode(Entity e);
    
    /**
     *  Prepares the nodes on the render graph for rendering using gfx.
     *
     *  @param  dt      The frame timestep, used for updating animations
     */
    void prepare(double dt);
    
    /**
     *  Render the node with the specified renderer
     *
     *  @param  renderer    The renderer containing shader and context used
     *                      for drawing a NodeGraph
     */
    void render(gfx::NodeRenderer& renderer);

private:
    cinek::gfx::NodeGraph _nodeGraph;
    
    struct Node
    {
        Entity entity;
        gfx::NodeHandle gfxNode;
        _NodeDelegate delegate;
    };
    
    //  nodes ordered by Entity
    std::vector<Node> _renderNodes;
    //
    //  index of last active & pending destroyed nodes in the nodelist
    //  new nodes are placed after _nodeEndIndex to the end of the rendernode
    //  list.  this is an optimization so that we can differentiate new vs
    //  active nodes in the list - and allows us to sort the list once per frame
    uint32_t _nodeEndIndex;
};
    
    }   /* namesapce ove */
}   /* namespace cinek */

#endif /* Overview_RenderGraph_hpp */
