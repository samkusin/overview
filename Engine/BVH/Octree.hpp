//
//  Graphs/Octree.hpp
//  Overview
//
//  Created by Samir Sinha on 6/12/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Graphs_Octree_hpp
#define Overview_Graphs_Octree_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/AABB.hpp"

#include <cinek/vector.hpp>

namespace cinek { namespace overview {

//  Implements a "Loose" Octree concept
//
//
//  References:
//      http://www.tulrich.com/geekstuff/partitioning.html
//      https://anteru.net/2008/11/14/315/
//
struct OctreeNode
{
    static constexpr int kMaxObjects = (sizeof(int32_t)*8) / sizeof(intptr_t);

    //  define the octant
    AABB<ckm::vec3> tightBounds;  // tight bounds of the octant
    AABB<ckm::vec3> looseBounds;  // loose bounds of the octant
    
    union
    {
        int32_t childIdx[8];
        intptr_t objects[kMaxObjects];
    };
    
    uint16_t numObjects;
    uint16_t flags;
    
    enum
    {
        kFlag_Valid         = 0x0001,
        kFlag_Leaf          = 0x0002
    };
};

class Octree
{
public:
    Octree(const AABB<Vector3>& bounds,
           int32_t nodeCount,
           const Allocator& allocator=Allocator());
    Octree(vector<OctreeNode>&& nodes);
    
    template<typename _Utility>
    int32_t insertObject(intptr_t objId, _Utility& util);
    
private:
    vector<OctreeNode> _nodes;

    
    template<typename _Utility>
    int32_t insertObjectAtNode(intptr_t objId, int32_t nodeIdx,
                               int32_t parentNodeIdx, _Utility& util);
};

inline Octree::Octree
(
    const AABB<Vector3>& bounds,
    int32_t nodeCount,
    const Allocator& allocator
) :
    _nodes(allocator)
{
    _nodes.reserve(nodeCount);
    _nodes.emplace_back();
    _nodes[0].tightBounds = bounds;
    _nodes[0].looseBounds = bounds;
    _nodes[0].flags = OctreeNode::kFlag_Valid | OctreeNode::kFlag_Leaf;
}

inline Octree::Octree(vector<OctreeNode>&& nodes) :
    _nodes(std::move(nodes))
{
}

template<typename _Utility>
int32_t Octree::insertObject
(
    intptr_t objId,
    _Utility& util
)
{
    if (_nodes.empty() || !(_nodes[0].flags & OctreeNode::kFlag_Valid))
        return -1;
    
    //  node 0 is our root
    return insertObjectAtNode(objId, 0, -1, util);
}

template<typename _Utility>
int32_t Octree::insertObjectAtNode
(
    intptr_t objId,
    int32_t nodeIdx,
    int32_t parentNodeIdx,
    _Utility& util
)
{
    int32_t nextNodeIdx = -1;
    
    //  if we're not a leaf, descend down to child octants
    if ((_nodes[nodeIdx].flags & OctreeNode::kFlag_Leaf)!=0)
    {
        auto& node = _nodes[nodeIdx];
        if (node.numObjects < OctreeNode::kMaxObjects)
        {
            node.objects[node.numObjects++] = objId;
            util.setObjectData(objId, nodeIdx);
            return nodeIdx;
        }
        else
        {
            //  if this node cannot fit the object, partition our leaf
            //  into 8 new leafs
        
        }
    }
    else
    {
        auto& node = _nodes[nodeIdx];
        
        //  choose octant
        //  select based off of 'ideal' or 'tight' bounds instead of 'loose'
        //  bounds. the chosen octant will be the object's container
        AABB<Vector3> octant;
        
        for (int i = 0; i < 8; ++i)
        {
            auto& childNode = _nodes[node.childIdx[i]];
            if (util.isObjectInBox(objId, childNode.tightBounds))
            {
                nextNodeIdx = insertObjectAtNode(
                    objId,
                    node.childIdx[i],
                    nodeIdx,
                    util
                );
                break;
            }
        }
    }
    
    //  extend parent bounds by our node's bounds
    if (parentNodeIdx >= 0)
    {
        _nodes[parentNodeIdx].looseBounds.merge(_nodes[nodeIdx].looseBounds);
    }
    
    return nextNodeIdx;
}

} /* namespace ovproto */ } /* namespace cinek */

#endif
