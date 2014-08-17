//
//  BlockCollection.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Builder/BlockCollection.hpp"

#include <algorithm>

namespace cinekine { namespace ovengine {

BlockCollection::BlockCollection(const char* name,
                                 vector<Block>&& blocks) :
    ModelCollection<Block>(name, std::move(blocks))
{
}


} /* namespace ovengine */ } /* namespace cinekine */
