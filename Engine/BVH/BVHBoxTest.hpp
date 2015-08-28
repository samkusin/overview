//
//  BVHBoxTest.hpp
//  Overview
//
//  Created by Samir Sinha on 8/19/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_BVH_BoxTest_hpp
#define Overview_BVH_BoxTest_hpp

namespace cinek { namespace overview {

template<typename _TreeType>
struct BVHBoxTest
{
    BVHBoxTest(const _TreeType& tree) : _tree(&tree) {}
    
    /// Sweeps the BVH in search of objects within a box
    ///
    /// @param  box         The AABB used to cull objects within the tree
    /// @param  callback    Callback issued for every object within the
    ///                     frustrum
    ///
    template<typename _Callback> int operator()(const ckm::AABB<ckm::vec3>& box,
        const _Callback& cb) const;

private:
    const _TreeType* _tree;
    
    template<typename _Callback>
    int testIntersect(const ckm::AABB<ckm::vec3>& box,
        int32_t atNodeIdx,
        const _Callback& cb) const;
};


template<typename _TreeType>
template<typename _Callback>
int BVHBoxTest<_TreeType>::operator()
(
    const ckm::AABB<ckm::vec3>& box,
    const _Callback& cb
)
const
{
    //  test the AABB of each node - do not descend if the box does not
    //  include the AABB
    //  test sphere against AABBs within tree.
    if (_tree->empty())
        return 0;
    return testIntersect(box, 0, cb);
}

template<typename _TreeType>
template<typename _Callback>
int BVHBoxTest<_TreeType>::testIntersect
(
    const ckm::AABB<ckm::vec3>& box,
    int32_t atNodeIdx,
    const _Callback& cb
) const
{
    auto& node = _tree->node(atNodeIdx);
    int cnt = 0;
    //  intersect sphere with node's AABB
    if (box.intersects(node.aabb))
    {
        if (node.isLeaf())
        {
            return cb(node.objectId, node.aabb) ? 1 : 0;
        }
        else
        {
            if (node.children.left >= 0)
            {
                cnt += testIntersect(box, node.children.left, cb);
            }
            if (node.children.right >= 0)
            {
                cnt += testIntersect(box, node.children.right, cb);
            }
        }
    }
    return cnt;
}


} /* namespace overview */ } /* namespace cinek */


#endif
