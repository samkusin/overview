//
//  BVHGraph.cpp
//  Overview
//
//  Created by Samir Sinha on 6/17/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "BVHGraph.hpp"

namespace cinek { namespace overview {

BVHTree::BVHTree(int32_t nodeCnt, const Allocator& allocator) :
    _nodes(allocator),
    _freeNodes(allocator)
{
    _nodes.reserve(nodeCnt);
    _freeNodes.reserve(nodeCnt);
}

int32_t BVHTree::allocateNode()
{
    int32_t nodeIdx = -1;
    if (!_freeNodes.empty())
    {
        nodeIdx = _freeNodes.back();
        _freeNodes.pop_back();
    }
    else
    {
        nodeIdx = (int32_t)_nodes.size();
        _nodes.emplace_back();
    }
    if (nodeIdx >= 0)
    {
        _nodes[nodeIdx].flags = BVHTree::NodeType::kFlag_Valid;
    }
    return nodeIdx;
}

void BVHTree::freeNode(int32_t nodeIdx)
{
    if (nodeIdx >= 0)
    {
        _nodes[nodeIdx].flags = 0;
    }
    _freeNodes.push_back(nodeIdx);
}

//  tests if at least one object intersects with the specified sphere
bool BVHTree::testIntersectWithSphere
(
    const Vector3& center,
    float radius,
    const Callback& cb
)
{
    //  test sphere against AABBs within tree.
    //  if at least one leaf intersects and passes the optional callback
    //  criteria, return true.
    if (_nodes.empty() || !_nodes[0].isValid())
        return false;
    return testIntersectWithSphere(center, radius, 0, cb);
}

bool BVHTree::testIntersectWithSphere
(
    const Vector3& center,
    float radius,
    int32_t atNodeIdx,
    const Callback& cb
)
{
    auto& node = _nodes[atNodeIdx];
    
    //  intersect sphere with node's AABB
    if (node.aabb.intersectsWithSphere(center, radius))
    {
        if (node.isLeaf())
        {
            return cb ? cb(node.objectId) : true;
        }
        else
        {
            if (node.children.left >= 0 &&
                testIntersectWithSphere(center, radius, node.children.left, cb))
                return true;
            if (node.children.right >= 0 &&
                testIntersectWithSphere(center, radius, node.children.right, cb))
                return true;
        }
    }
    return false;
}

} /* namespace overview */ } /* namespace cinek */

