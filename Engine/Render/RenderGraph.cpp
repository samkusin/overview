//
//  RenderGraph.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/20/15.
//
//

#include "RenderGraph.hpp"
#include "CKGfx/NodeRenderer.hpp"
#include "CKGfx/AnimationController.hpp"

#include <algorithm>

namespace cinek {
    namespace ove {

RenderGraph::RenderGraph
(
    const gfx::NodeElementCounts& counts,
    uint32_t entityCount,
    uint32_t animCount
) :
    _nodeGraph(counts),
    _animControllerPool(animCount)
{
    _renderNodes.reserve(entityCount);
    _nodeEndIndex = 0;
    _removedNodeCount = 0;
    
    _animNodes.reserve(animCount);
}

gfx::NodeHandle RenderGraph::cloneAndAddNode
(
    Entity e,
    gfx::NodeHandle sourceNode,
    void* context
)
{
    auto rootNode = _nodeGraph.root();
    if (!rootNode)
        return nullptr;
        
    auto parentNode = _nodeGraph.createObjectNode(e);
    _nodeGraph.addChildNodeToNode(_nodeGraph.clone(sourceNode), parentNode);
    _nodeGraph.addChildNodeToNode(parentNode, rootNode);
    
    //  add to end of render nodes list for sort during prepare()
    //  note, this may leads to duplicates
    _renderNodes.emplace_back(Node{ e, parentNode, context });
    return parentNode;
}

gfx::NodeHandle RenderGraph::setNodeEntity(Entity e, gfx::NodeHandle h)
{
    _renderNodes.emplace_back(Node{ e, h, nullptr });
    return h;
}

void RenderGraph::removeNode(Entity e)
{
    //  search within our sorted region first
    struct comparator
    {
        bool operator()(const Node& n, Entity e) const {
            return n.entity < e;
        }
        bool operator()(Entity e, const Node& n) const {
            return e < n.entity;
        }
    };
    auto itRange = std::equal_range(_renderNodes.begin(),
                        _renderNodes.begin() + _nodeEndIndex, e, comparator());
    if (itRange.first == itRange.second) {
        // not inside the sorted region, check the added node region, which is
        // just a vector of added, not sorted, nodes
        for (auto it = _renderNodes.begin() + _nodeEndIndex; it != _renderNodes.end(); ) {
            auto& node = *it;
            
            if (node.entity == e) {
                _nodeGraph.removeNode(node.gfxNode);
                it = _renderNodes.erase(it);
            }
            else {
                ++it;
            }
        }
        return;
    }
    
    //  do not clear entity in the sorted list - we'll clear during prepare()
    for (auto it = itRange.first; it != itRange.second; ++it) {
        _nodeGraph.removeNode(it->gfxNode);
        it->gfxNode = nullptr;
        it->context = nullptr;
        ++_removedNodeCount;
    }
}

void RenderGraph::clear()
{
    //  NOTE - this might be inefficient if our intent is to reset the NodeGraph
    //  A NodeGraph destroy will invalidate all handles in our _renderNodes
    //  vector.  But resetting NodeGraph before clearing _renderNodes will
    //  result in undefined behavior when the Nodes in _renderNodes are
    //  destroyed.
    //
    //  Instead of storing gfx::NodeHandles in our internal Node object ,we
    //  could reference gfx::NodeHandle by a pointer to gfx::Node it manages,
    //  increment its ref when adding, decrement its ref when removing, but
    //  when *clearing*, do the following:
    //
    //      - Destroy the gfx::NodeGraph (which internally isn't trivial.)
    //      - Use the 'Component Resource' Model:
    //          - When adding and removing nodes, 'link' and 'unlink'
    //            NodeHandles using their respective methods
    //          - Reference gfx::Nodes by the returned pointer
    //          - When invalidating the _renderNodes vector, since we use POD
    //            destructor code is not referenced
    //
    // Again, our current approach *might* be inefficient.  We'll get back to
    // this after profiling.
    //
    _animNodes.clear();
    _renderNodes.clear();
    _nodeGraph.clearRoot();
}

void RenderGraph::updateRenderNodes(double dt)
{
    //  resort vector to include any added nodes (at end of list)
    //  sort removed nodes to the end of the vector and remove them
    //  note - this can be optimized (sort added vectors only, then merge them
    //  with the existing sorted active vector.)
    //
    if (_renderNodes.size() >= _nodeEndIndex || _removedNodeCount >=_nodeEndIndex/kRemoveNodeSortFactor) {
        std::sort(_renderNodes.begin(), _renderNodes.end(),
                [](const Node& n0, const Node& n1) -> bool {
                    return ((n0.entity < n1.entity) && n0.gfxNode) ||
                           ((n1.entity >= n0.entity) && !n1.gfxNode);
                });
        
        auto it = std::find_if(_renderNodes.begin(), _renderNodes.end(),
                [](const Node& n) -> bool { return !n.gfxNode; });
        
        _renderNodes.erase(it, _renderNodes.end());
        _nodeEndIndex = (uint32_t)_renderNodes.size();
        _removedNodeCount = 0;
    }
    
    //  animate controllers
    for (auto& animNode : _animNodes) {
        animNode.animController->update(_renderTime);
    }

    _renderTime += dt;
}

gfx::NodeHandle RenderGraph::root() const
{
    return _nodeGraph.root();
}

    
    }   /* namesapce ove */
}   /* namespace cinek */