//
//  StellarSystem.cpp
//  Overview
//
//  Created by Samir Sinha on 7/31/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "StellarSystem.hpp"

namespace cinek { namespace ovproto {

COMPONENT_TYPE_IMPL(StellarSystemComponent, kStellarSystemComponent,
    {
        _indexToTreeNode = -1;
    }
);

void StellarSystemComponent::init(uint32_t seed, ckm::scalar radius)
{
    _seed = seed;
    _radius = radius;
}

} /* namespace overview */ } /* namespace cinek */
