//
//  StellarSystem.cpp
//  Overview
//
//  Created by Samir Sinha on 7/31/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "StellarSystem.hpp"

namespace cinek { namespace ovproto {

namespace component
{
    COMPONENT_TYPE_IMPL(StellarSystem, kStellarSystem,
        {
            _indexToTreeNode = -1;
        }
    );

    void StellarSystem::init(uint32_t seed, ckm::scalar radius)
    {
        _seed = seed;
        _radius = radius;
    }
    
}

} /* namespace overview */ } /* namespace cinek */
