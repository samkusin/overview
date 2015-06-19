//
//  Graphs/BVHTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 5/16/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Graphs_BVHTypes_hpp
#define Overview_Graphs_BVHTypes_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/AABB.hpp"

namespace cinek { namespace overview {

//  The BVHNode uses indices into arrays for references to BVHNode objects
//
struct BVHNode
{
    enum
    {
        kFlag_Valid     = 0x80000000,
        kFlag_Leaf      = 0x40000000
    };

    int32_t parent = -1;
    uint32_t flags = 0;

    AABB<Vector3> aabb;
    
    union           // 8 bytes max
    {
        struct
        {
            int32_t left;
            int32_t right;
        }
        children;
        
        intptr_t objectId;
    };
   
    void initAsLeaf()
    {
        flags = kFlag_Valid + kFlag_Leaf;
        objectId = 0;
    }
    
    void initAsFork()
    {
        flags = kFlag_Valid;
        children.left = children.right = -1;
    }
    
    bool isValid() const { return (flags & kFlag_Valid)!=0; }
    bool isLeaf() const { return (flags & kFlag_Leaf)!=0; }
};


} /* namespace overview */ } /* namespace cinek */

#endif
