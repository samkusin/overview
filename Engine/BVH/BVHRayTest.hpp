//
//  BVHRayTest.hpp
//  Overview
//
//  Created by Samir Sinha on 6/23/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_BVH_RayTest_hpp
#define Overview_BVH_RayTest_hpp

#include "Engine/EngineTypes.hpp"

namespace cinek { namespace overview {

template<typename _TreeType>
struct BVHTestIntersectWithSphere
{
    BVHTestIntersectWithSphere(const _TreeType& tree) : _tree(&tree) {}
    
    template<typename _Callback> bool operator()(const ckm::vec3& center,
        ckm::vec3::value_type radius,
        const _Callback& cb) const;
    
    bool operator()(const ckm::vec3& center, ckm::vec3::value_type radius) const;

private:
    const _TreeType* _tree;
    
    template<typename _Callback>
    bool testIntersect(const ckm::vec3& center,
        ckm::vec3::value_type radius,
        int32_t atNodeIdx,
        const _Callback& cb) const;
    
    bool testIntersect(const ckm::vec3& center,
        ckm::vec3::value_type radius,
        int32_t atNodeIdx) const;
};

template<typename _TreeType>
bool BVHTestIntersectWithSphere<_TreeType>::operator()
(
    const ckm::vec3& center,
    ckm::vec3::value_type radius
)
const
{
    //  test sphere against AABBs within tree.
    //  if at least one leaf intersects and passes the optional callback
    //  criteria, return true.
    if (_tree->empty())
        return false;
    return testIntersect(center, radius, 0);
}

template<typename _TreeType>
template<typename _Callback>
bool BVHTestIntersectWithSphere<_TreeType>::operator()
(
    const ckm::vec3& center,
    ckm::vec3::value_type radius,
    const _Callback& cb
)
const
{
    //  test sphere against AABBs within tree.
    //  if at least one leaf intersects and passes the optional callback
    //  criteria, return true.
    if (_tree->empty())
        return false;
    return testIntersect(center, radius, 0, cb);
}

template<typename _TreeType>
template<typename _Callback>
bool BVHTestIntersectWithSphere<_TreeType>::testIntersect
(
    const ckm::vec3& center,
    ckm::vec3::value_type radius,
    int32_t atNodeIdx,
    const _Callback& cb
) const
{
    auto& node = _tree->node(atNodeIdx);
    
    //  intersect sphere with node's AABB
    if (node.intersectsWithSphere(center, radius))
    {
        if (node.isLeaf())
        {
            return cb(node.objectId);
        }
        else
        {
            if (node.children.left >= 0 &&
                testIntersect(center, radius, node.children.left, cb))
                return true;
            if (node.children.right >= 0 &&
                testIntersect(center, radius, node.children.right, cb))
                return true;
        }
    }
    return false;
}

template<typename _TreeType>
bool BVHTestIntersectWithSphere<_TreeType>::testIntersect
(
    const ckm::vec3& center,
    ckm::vec3::value_type radius,
    int32_t atNodeIdx
)
const
{
    auto& node = _tree->node(atNodeIdx);
    
    //  intersect sphere with node's AABB
    if (node.intersectsWithSphere(center, radius))
    {
        if (node.isLeaf())
        {
            return true;
        }
        else
        {
            if (node.children.left >= 0 &&
                testIntersect(center, radius, node.children.left))
                return true;
            if (node.children.right >= 0 &&
                testIntersect(center, radius, node.children.right))
                return true;
        }
    }
    return false;

}


} /* namespace overview */ } /* namespace cinek */

#endif /* defined(Overview_BVH_RayTest_hpp) */
