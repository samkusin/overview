//
//  BVHFrustrumTest.hpp
//  Overview
//
//  Created by Samir Sinha on 7/6/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_BVH_FrustrumTest_hpp
#define Overview_BVH_FrustrumTest_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/EngineMath.hpp"
#include "Engine/EngineGeometry.hpp"

#include <array>

namespace cinek { namespace overview {

template<typename _TreeType>
struct BVHTestFrustrumSweep
{
    BVHTestFrustrumSweep(const _TreeType& tree) : _tree(&tree) {}
    
    /// The frustrum consists of a near Z and far Z distance from the specfied
    /// view point, field of view, and aspect ratio.
    ///
    /// Since objects in the specified tree might be in a different coordinate
    /// space (i.e. world) than the view frustrum, callers pass a rotation
    /// matrix used to transform the view frustrum into the tree's world space.
    ///
    /// @param  frustrum  The frustrum used to cull objects within the tree
    /// @param  callback  Callback issued for every object within the frustrum
    ///                   with signature (ObjectId, const AABB<vec3>&)
    ///
    template<typename _Callback> int operator()(const ckm::Frustrum& frustrum,
        const _Callback& cb) const;
    
    template<typename _Callback> int operator()(const ckm::Frustrum& frustrum,
        ckm::Frustrum::Plane plane,
        const _Callback& cb) const;

private:
    const _TreeType* _tree;
    
    template<typename _Callback>
    int testIntersect(const ckm::Frustrum& frustrum,
        int32_t atNodeIdx,
        const _Callback& cb) const;
    
    template<typename _Callback>
    int testIntersect(const ckm::Frustrum& frustrum,
        ckm::Frustrum::Plane plane,
        int32_t atNodeIdx,
        const _Callback& cb) const;
};


template<typename _TreeType>
template<typename _Callback>
int BVHTestFrustrumSweep<_TreeType>::operator()
(
    const ckm::Frustrum& frustrum,
    const _Callback& cb
)
const
{
    //  test the AABB of each node - do not descend if the frustrum does not
    //  include the AABB
    //  test sphere against AABBs within tree.
    if (_tree->empty())
        return 0;
    return testIntersect(frustrum, 0, cb);
}

template<typename _TreeType>
template<typename _Callback>
int BVHTestFrustrumSweep<_TreeType>::operator()
(
    const ckm::Frustrum& frustrum,
    ckm::Frustrum::Plane plane,
    const _Callback& cb
)
const
{
    //  test the AABB of each node - do not descend if the frustrum does not
    //  include the AABB
    //  test sphere against AABBs within tree.
    if (_tree->empty())
        return 0;
    return testIntersect(frustrum, plane, 0, cb);
}

template<typename _TreeType>
template<typename _Callback>
int BVHTestFrustrumSweep<_TreeType>::testIntersect
(
    const ckm::Frustrum& frustrum,
    int32_t atNodeIdx,
    const _Callback& cb
) const
{
    auto& node = _tree->node(atNodeIdx);
    int cnt = 0;
    //  intersect sphere with node's AABB
    if (frustrum.testAABB(node.aabb))
    {
        if (node.isLeaf())
        {
            return cb(node.objectId, node.aabb) ? 1 : 0;
        }
        else
        {
            if (node.children.left >= 0)
            {
                cnt += testIntersect(frustrum, node.children.left, cb);
            }
            if (node.children.right >= 0)
            {
                cnt += testIntersect(frustrum, node.children.right, cb);
            }
        }
    }
    return cnt;
}

template<typename _TreeType>
template<typename _Callback>
int BVHTestFrustrumSweep<_TreeType>::testIntersect
(
    const ckm::Frustrum& frustrum,
    ckm::Frustrum::Plane plane,
    int32_t atNodeIdx,
    const _Callback& cb
) const
{
    auto& node = _tree->node(atNodeIdx);
    int cnt = 0;
    //  intersect sphere with node's AABB
    if (frustrum.testAABBWithPlane(node.aabb, plane))
    {
        if (node.isLeaf())
        {
            return cb(node.objectId, node.aabb) ? 1 : 0;
        }
        else
        {
            if (node.children.left >= 0)
            {
                cnt += testIntersect(plane, node.children.left, cb);
            }
            if (node.children.right >= 0)
            {
                cnt += testIntersect(plane, node.children.right, cb);
            }
        }
    }
    return cnt;
}


} /* namespace overview */ } /* namespace cinek */

#endif
