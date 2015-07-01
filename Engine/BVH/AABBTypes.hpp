//
//  BVH/AABBTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 5/16/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_BVH_AABBTypes_hpp
#define Overview_BVH_AABBTypes_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/AABB.hpp"

#include <glm/vec3.hpp>

namespace cinek { namespace overview {

//  The BVHNode uses indices into arrays for references to BVHNode objects
//
template<typename _ObjectId>
struct AABBNode
{
    using ObjectId = _ObjectId;
    using Index = int32_t;
    
    enum
    {
        kFlag_Valid     = 0x80000000,
        kFlag_Leaf      = 0x40000000
    };

    Index parent = -1;
    uint32_t flags = 0;

    AABB<ckm::vec3> aabb;
    
    union           // 8 bytes max
    {
        struct
        {
            Index left;
            Index right;
        }
        children;
        
        ObjectId objectId;
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

    bool intersectsWithSphere(const ckm::vec3& center, ckm::vec3::value_type radius) const {
        return aabb.intersectsWithSphere(center, radius);
    }
};


} /* namespace overview */ } /* namespace cinek */

#endif
