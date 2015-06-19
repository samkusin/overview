//
//  Graphs/BVHGraph.hpp
//  Overview
//
//  Created by Samir Sinha on 5/16/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Graphs_BVHGraph_hpp
#define Overview_Graphs_BVHGraph_hpp

#include "BVHTypes.hpp"

#include <cinek/vector.hpp>
#include <functional>

namespace cinek { namespace overview {

//  Let's give credit to Bullet and other physics engines that use BVH
//  for collision and raycasting.  Most implementations end up referencing
//  Bullet or educational resources.

//  BVHTree
//
//  Specialized for trees that are infrequently rebalanced (i.e. static)
//  Balancing starts with a single AABB and all objects, digging down until
//  all objects are placed within BVH leaves, where each leaf's object count
//  does not exceed a certain threshold count.
//

//  Type _Object must implement the following:
//
//  struct _Utility
//  {
//      void setObjectData(intptr_t objId, intptr_t data);
//      float objectRadius(intptr_t objId) const;
//      Vector3 position(intptr_t objId) const;
//  };
//

class BVHTree
{
public:
    using NodeType = BVHNode;
    
    BVHTree() = default;
    BVHTree(int32_t nodeCnt, const Allocator& allocator=Allocator());
    
    //  adds an object to the hierarchy
    template<typename _Utility>
    int32_t insertObject(intptr_t objectId, _Utility& util);
    
    using Callback = std::function<bool(intptr_t)>;
    
    //  tests if at least one object intersects with the specified sphere
    bool testIntersectWithSphere(const Vector3& center, float radius,
        const Callback& cb=Callback());
    
private:
    template<typename _Utility>
    int32_t insertObjectIntoBVH(intptr_t objectId,
        int32_t atNodeIdx,
        int32_t parentNodeIdx,
        _Utility& util);
    
    int32_t allocateNode();
    void freeNode(int32_t node);
    
    template<typename _Utility>
    AABB<Vector3> makeAABBForObject(intptr_t objectId, _Utility& util) const;
    
    bool testIntersectWithSphere(const Vector3& center, float radius,
        int32_t atNodeIdx,
        const Callback& cb);

    //  graph data.
    vector<NodeType> _nodes;
    vector<int32_t> _freeNodes;
};


template<typename _Utility>
AABB<Vector3> BVHTree::makeAABBForObject(intptr_t objectId, _Utility& util) const
{
    AABB<Vector3> aabb(util.objectRadius(objectId));
    aabb += util.position(objectId);
    return aabb;
}


template<typename _Utility>
int32_t BVHTree::insertObject(intptr_t objectId, _Utility& util)
{
    int32_t nodeIdx = -1;
    if (!_nodes.empty())
    {
        nodeIdx = 0;
    }
    return insertObjectIntoBVH(objectId, nodeIdx, -1, util);
}

template<typename _Utility>
int32_t BVHTree::insertObjectIntoBVH(intptr_t objectId,
        int32_t atNodeIdx,
        int32_t parentNodeIdx,
        _Utility& util)
{
    if (atNodeIdx < 0)
    {
        //  allocate leaf
        atNodeIdx = allocateNode();
        if (atNodeIdx < 0)
            return atNodeIdx;

        auto& node = _nodes[atNodeIdx];
        
        node.initAsLeaf();
        node.objectId = objectId;
        
        node.aabb = makeAABBForObject(objectId, util);
        util.setObjectData(objectId, atNodeIdx);
    }
    else
    {
        auto& node = _nodes[atNodeIdx];

        intptr_t children[2];
        
        if (node.isLeaf())
        {
            util.setObjectData(node.objectId, -1);
            children[0] = node.objectId;
            children[1] = objectId;
            node.initAsFork();
        }
        else
        {
            //  use bounding volumes of child nodes to determine which
            //  bounding volume to add this object to
            //  compare the AABBs of the children *combined* with the object's
            //  AABB.  The smaller of the two AABBs will be chosen.
            
            AABB<Vector3> leftAABB = _nodes[node.children.left].aabb;
            AABB<Vector3> rightAABB = _nodes[node.children.right].aabb;
            AABB<Vector3> objectAABB = makeAABBForObject(objectId, util);
            leftAABB.merge(objectAABB);
            rightAABB.merge(objectAABB);
            
            if (leftAABB.volume() < rightAABB.volume())
            {
                children[0] = objectId;
                children[1] = -1;
            }
            else
            {
                children[0] = -1;
                children[1] = objectId;
            }
        }
        
        //  note - we do not
        if (children[0] >= 0)
        {
            _nodes[atNodeIdx].children.left = insertObjectIntoBVH(children[0],
                _nodes[atNodeIdx].children.left,
                atNodeIdx,
                util);
        }
        if (children[1] >= 0)
        {
            _nodes[atNodeIdx].children.right = insertObjectIntoBVH(children[1],
                _nodes[atNodeIdx].children.right,
                atNodeIdx,
                util);
        }
    }
    
    //  update our parent's AABB (remember, we're recursively calling this
    //  method.
    if (parentNodeIdx >= 0)
    {
        _nodes[parentNodeIdx].aabb.merge(_nodes[atNodeIdx].aabb);
    }
    
    _nodes[atNodeIdx].parent = parentNodeIdx;
    
    return atNodeIdx;
}

/*

//  returns the index of the created node.
//  algo adapted from 3dmuve.com
//
template<typename _ObjectIdType, typename _ObjectUtility>
int32_t partitionBVH
(
    BVHStaticGraph<_ObjectIdType>& graph,
    int32_t parentNodeIdx,
    const std::pair<int32_t, int32_t>& objectRange,
    BVHPartitionPlane::Axis axis
)
{
    using ObjectId = _ObjectIdType;
    auto& nodes = graph.nodes;
    auto& objects = graph.objects;
    
    //  create a new node - it will either be a leaf, or a branch.
    int32_t nodeIndex = (int32_t)nodes.size();
    nodes.emplace_back();
    if (nodeIndex == (int32_t)nodes.size())
        return -1;
    
    nodes.back().parent = parentNodeIdx;
    
    auto objCount = (objectRange.second - objectRange.first);
    
    if (objCount < _ObjectsPerNode)
    {
        auto node = &nodes.back();
        
        node->flags |= BVHNode<ObjectId>::kFlag_Leaf;
        
        node->objects.start = objectRange.first;
        node->objects.count = objCount;
        
        //  could be an empty leaf
        if (objCount > 0)
        {
            //  define aabb emcompassing all objects
            auto& nodeAABB = node->aabb;
            
            auto& objId = objects[objectRange.first];
            
            _ObjectUtility::setImplData(objId);

            auto& objHalfDim = _ObjectUtility::halfDimensions(objId);
            auto& transform = _ObjectUtility::transform(objId);
            bx::vec3Neg(nodeAABB.min, objHalfDim);
            nodeAABB.max = objHalfDim;
            nodeAABB.rotate(transform.matrix);
            
            for (int32_t i = 1; i < objCount; ++i)
            {
                objId = objects[objectRange.first+i];
                _ObjectUtility::setImplData(objId);
                AABB<Vector3> aabb(_ObjectUtility::halfDimensions(objId));
                aabb.rotate(_ObjectUtility::transform(objId).matrix);
                nodeAABB.merge(aabb);
            }
            
            //  update parent AABB to *at least* encompass this node's AABB.
            if (parentNodeIdx >= 0)
            {
                nodes[parentNodeIdx].aabb.merge(nodeAABB);
            }
        }
    }
    else
    {
        //  sort indices in place within our object range along an axis
        std::sort(objects.begin()+objectRange.first,
                  objects.begin()+objCount,
                  [axis](const ObjectId& l, const ObjectId& r) -> bool {
                    return _ObjectUtility::compare(l, r);
                  });
        
        //  define our AABB based on the first object's AABB. via recusion,
        //  our child nodes will grow our AABB, instead of iteratively doing it
        //  here.
        auto& object = objects[objectRange.first];
        auto& objHalfDim = _ObjectUtility::halfDimensions(object);
        
        bx::vec3Neg(nodes[nodeIndex].aabb.min, objHalfDim);
        nodes[nodeIndex].aabb.max = objHalfDim;
        nodes[nodeIndex].aabb.rotate(_ObjectUtility::transform().matrix);
        
        //  partition into two child nodes using the mid object as a pivot.
        //  this algo could be improved to take distance into account.
        auto centerOffs = objCount/2;
        
        nodes[nodeIndex].children.left = partitionBVH(graph, nodeIndex,
            std::make_pair(objectRange.first, objectRange.first+centerOffs),
            BVHPartitionPlane::next(axis)
        );
        nodes[nodeIndex].children.right = partitionBVH(graph, nodeIndex,
            std::make_pair(objectRange.first+centerOffs+1, objectRange.second),
            BVHPartitionPlane::next(axis)
        );
        
        //  update our parent's AABB (remember, we're recursively calling this
        //  method.
        if (parentNodeIdx >= 0)
        {
            nodes[parentNodeIdx].aabb.merge(nodes[nodeIndex].aabb);
        } 
    }
    return nodeIndex;
}
*/

} /* namespace overview */ } /* namespace cinek */

#endif /* defined(Overview_BVH_StaticGraph_hpp) */
