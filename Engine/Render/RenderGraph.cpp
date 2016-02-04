//
//  RenderGraph.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/20/15.
//
//

#include "RenderGraph.hpp"
#include "Engine/EntityDatabase.hpp"
#include "Engine/Debug.hpp"
#include "CKGfx/NodeRenderer.hpp"
#include "CKGfx/Node.hpp"
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
    _animControllerPool(animCount),
    _nodeGraph(counts),
    _renderTime(0)
{
    _pendingRenderNodes.reserve(entityCount);
    _removedRenderNodes.reserve(entityCount);
    _renderNodes.reserve(entityCount);
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
        
    auto clonedNode = _nodeGraph.clone(sourceNode);
    
    struct VisitContext
    {
        RenderGraph* self;
        Entity e;
    };
    
    VisitContext vc { this, e };
    
    //  initialize any rendering subsystems needed by the attached node
    cinek::gfx::visit(clonedNode, [&vc](cinek::gfx::NodeHandle node) -> bool {
        if (node->elementType() == cinek::gfx::Node::kElementTypeArmature) {
            cinek::gfx::AnimationController controller(node->armature()->animSet);
            
            auto animController = vc.self->_animControllerPool.add(std::move(controller));
            node->armature()->animController = animController;
            
            vc.self->addAnimNode(vc.e, animController);
            animController->transitionToState("Walk");
        }
        return true;
    });
    
    //  our pending node will be added during the update() call
    auto parentNode = _nodeGraph.createObjectNode(e);
    parentNode->setTransform(gfx::Matrix4::kIdentity);
    _nodeGraph.addChildNodeToNode(clonedNode, parentNode);
    _nodeGraph.addChildNodeToNode(parentNode, rootNode);
    
    _pendingRenderNodes.emplace_back(Node{ e, parentNode, context });
    
    return parentNode;
}

gfx::NodeHandle RenderGraph::setNodeEntity(Entity e, gfx::NodeHandle h)
{
    _pendingRenderNodes.emplace_back(Node{ e, h, nullptr });
    return h;
}

void RenderGraph::removeNode(Entity e)
{
    _removedRenderNodes.emplace_back(e);
}

gfx::NodeHandle RenderGraph::findNode(Entity entity) const
{
    //  search sorted active list first
    //  then check the pending list
    //  otherwise bail (removed nodes are not included)
    
    auto it = std::lower_bound(_renderNodes.begin(), _renderNodes.end(), entity,
        [](const Node& node, Entity e) -> bool {
            return (node.entity < e);
        });
    if (it == _renderNodes.end() || it->entity != entity) {
        //  pending list is not sorted
        it = std::find_if(_pendingRenderNodes.begin(), _pendingRenderNodes.end(),
            [entity](const Node& node) -> bool {
                return node.entity == entity;
            });
    }
    return it != _renderNodes.end() ? it->gfxNode : nullptr;
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
    _pendingRenderNodes.clear();
    _animNodes.clear();
    _renderNodes.clear();
    _removedRenderNodes.clear();
    _nodeGraph.clearRoot();
}

void RenderGraph::update
(
    double dt
)
{
    //  sort added nodes into active list first
    if (!_pendingRenderNodes.empty()) {
        _renderNodes.insert(_renderNodes.end(), _pendingRenderNodes.begin(), _pendingRenderNodes.end());
        
        std::sort(_renderNodes.begin(), _renderNodes.end(),
                [](const Node& n0, const Node& n1) -> bool {
                    return (n0.entity < n1.entity);
                });
        
        _pendingRenderNodes.clear();
    }

    //  remove nodes using our remove list
    if (!_removedRenderNodes.empty()) {
        //  sort our remove list so we can iterate through both active and remove
        //  linearly
        std::sort(_removedRenderNodes.begin(), _removedRenderNodes.end(),
                [](Entity e0, Entity e1) -> bool {
                    return e0 < e1;
                });
        
        auto toRemoveIt = _removedRenderNodes.begin();
        auto activeIt = _renderNodes.begin();
        while (toRemoveIt != _removedRenderNodes.end() && activeIt != _renderNodes.end()) {
            Entity entityToRemove = *toRemoveIt;
            activeIt = std::lower_bound(activeIt, _renderNodes.end(), entityToRemove,
                [](const Node& n0, Entity e) -> bool {
                    return n0.entity < e;
                });
            
            if (activeIt != _renderNodes.end()) {
                if (activeIt->entity == entityToRemove) {
                    _nodeGraph.detachNodeTree(activeIt->gfxNode);
                    activeIt = _renderNodes.erase(activeIt);
                }
                else {
                    OVENGINE_LOG_WARN("Attempt to remove a non-active entity %" PRIu64 ".", entityToRemove);
                    ++activeIt;
                }
            }
            ++toRemoveIt;
        }
    }

    //  animate controllers, removing dead controllers as needed
    //  we can use a similar approach to the one above since the animations
    //  vector is also sorted by entity
    auto toRemoveIt = _removedRenderNodes.begin();
    for (auto it = _animNodes.begin(); it != _animNodes.end();) {
        Entity thisEntity = it->entity;
        
        while (toRemoveIt != _removedRenderNodes.end() && *toRemoveIt < thisEntity) {
            ++toRemoveIt;
        }
        
        if (toRemoveIt == _removedRenderNodes.end() || *toRemoveIt != thisEntity) {
            it->animController->update(_renderTime);
            ++it;
        }
        else {
            it = _animNodes.erase(it);
        }
    }

    _removedRenderNodes.clear();

    _renderTime += dt;
}

gfx::NodeHandle RenderGraph::root() const
{
    return _nodeGraph.root();
}


auto RenderGraph::addAnimNode
(
    Entity e,
    gfx::AnimationControllerHandle h
) -> AnimNode*
{
    auto it = std::lower_bound(_animNodes.begin(), _animNodes.end(), e,
        [](const AnimNode& animNode, Entity e) -> bool {
            return animNode.entity < e;
        });
    CK_ASSERT_RETURN_VALUE(it == _animNodes.end() || it->entity != e, &(*it));
    
    AnimNode node { e, h };
    it = _animNodes.emplace(it, std::move(node));
    
    return &(*it);
}


    
    }   /* namesapce ove */
}   /* namespace cinek */