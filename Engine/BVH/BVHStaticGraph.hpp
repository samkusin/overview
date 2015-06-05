//
//  BVHStaticGraph.hpp
//  Overview
//
//  Created by Samir Sinha on 5/16/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_BVH_StaticGraph_hpp
#define Overview_BVH_StaticGraph_hpp

#include "BVHTypes.hpp"

#include <cinek/vector.hpp>

namespace cinek { namespace overview {

//  Let's give credit to Bullet and other physics engines that use BVH
//  for collision and raycasting.  Most implementations end up referencing
//  Bullet or educational resources.

//  BVHStaticGraph
//
//  Specialized for trees that are infrequently rebalanced (i.e. static)
//  Balancing starts with a single AABB and all objects, digging down until
//  all objects are placed within BVH leaves, where each leaf's object count
//  does not exceed a certain threshold count.
//

//  Type _Object must implement the following:
//
//  struct _ObjectUtility
//  {
//      static bool compare(_ObjectIdType l, _ObjectIdType r);
//      static const Vector3& halfDimensions(_ObjectIdType objId);
//      static const Matrix4& transform(_ObjectIdType objId);
//      static void setImplData(_ObjectIdType objId, intptr_t data);
//  };
//

template<typename _ObjectIdType>
struct BVHStaticGraph
{
    using ObjectIdType = _ObjectIdType;
  
    vector<BVHNode<ObjectIdType>> nodes;
    vector<ObjectIdType> objects;
};

template<typename _ObjectIdType, int _ObjectsPerNode, typename _ObjectUtility>
BVHStaticGraph<_ObjectIdType> generateBVH(vector<_ObjectIdType> objects);


template<typename _ObjectIdType, int _ObjectsPerNode, typename _ObjectUtility>
int32_t partitionBVH(BVHStaticGraph<_ObjectIdType>& graph,
    int32_t parentNodeIdx,
    const std::pair<int32_t, int32_t>& objectRange,
    BVHPartitionPlane::Axis axis
);


template<typename _ObjectIdType, int _ObjectsPerNode, typename _ObjectUtility>
BVHStaticGraph<_ObjectIdType> generateBVH(vector<_ObjectIdType> objects)
{
    BVHStaticGraph<_ObjectIdType> graph;
 
    graph.nodes.reserve(objects.size());
    graph.objects = std::move(objects);
    
    partitionBVH<_ObjectIdType, _ObjectsPerNode, _ObjectUtility>(
        graph,
        -1,
        std::make_pair(0, (int32_t)objects.size()),
        BVHPartitionPlane::kX
    );
    
    return graph;
}

//  returns the index of the created node.
//  algo adapted from 3dmuve.com
//
template<typename _ObjectIdType, int _ObjectsPerNode, typename _ObjectUtility>
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


} /* namespace overview */ } /* namespace cinek */

#endif /* defined(Overview_BVH_StaticGraph_hpp) */
