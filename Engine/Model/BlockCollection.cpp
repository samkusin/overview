//
//  BlockCollection.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Model/BlockCollection.hpp"

#include <algorithm>

namespace cinekine { namespace ovengine {

BlockCollection::BlockCollection(const char* name,
                                 const char* tileCollectionName,
                                 vector<Block>&& blocks) :
    ModelCollection<Block>(name, std::move(blocks)),
    _tileCollectionName(tileCollectionName)
{
}


} /* namespace ovengine */ } /* namespace cinekine */
