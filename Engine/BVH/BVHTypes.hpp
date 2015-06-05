//
//  BVHTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 5/16/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_BVH_Types_hpp
#define Overview_BVH_Types_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/AABB.hpp"

namespace cinek { namespace overview {

struct BVHPartitionPlane
{
    enum Axis
    {
        kX,
        kY,
        kZ
    };
    
    static Axis next(Axis axis)
    {
        if (axis == kX)
            return kY;
        else if (axis == kY)
            return kZ;
        else
            return kX;
    }
};

//  The BVHNode uses indices into arrays for references to BVHNode objects
//
template<typename _ObjectIdType>
struct BVHNode
{
    enum
    {
        kFlag_Leaf      = 0x80000000
    };

    int32_t parent = -1;
    uint32_t flags = 0;
    
    union
    {
        struct
        {
            int32_t left;
            int32_t right;
        }
        children;
        
        struct
        {
            int32_t start;
            int32_t count;
        }
        objects;
    };
    
    AABB<Vector3> aabb;
};


} /* namespace ovproto */ } /* namespace cinek */

#endif
