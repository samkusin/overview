//
//  EntityDefs.hpp
//  Overview
//
//  Created by Samir Sinha on 9/15/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_EntityDefs_hpp
#define Overview_EntityDefs_hpp

typedef uint64_t CKEntity;
typedef uint16_t CKEntityIteration;
typedef uint16_t CKEntityContext;
typedef uint32_t CKEntityIndex;

enum
{
    kCKEntityIndexMask      = 0x00000000ffffffff,
    kCKEntityIndexBits      = 32,
    kCKEntityIterationMask  = 0x0000ffff00000000,
    kCKEntityIterationBits  = 16,
    kCKEntityContextMask    = 0xffff000000000000,
    kCKEntityContextBits    = 16
};


#endif
