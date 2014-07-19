//
//  Block.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Model/Block.hpp"

namespace cinekine { namespace ovengine {

Block::Block() :
    _granluarity(0)
{
}

Block::Block(const char* name, int granularity, const Allocator& allocator) :
    _name(name),
    _granluarity(granularity)
{
    _grids[kClass_1x1] = createGrid(1);
    _grids[kClass_3x3] = createGrid(3);
}


TileGrid Block::createGrid(int dimension, const Allocator& allocator)
{
    return std::move(TileGrid(dimension*_granularity, dimension*_granularity,
                              allocator));
}

}   /* namespace ovengine */ }  /* namespace cinekine */
