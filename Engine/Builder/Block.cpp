//
//  Block.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Builder/Block.hpp"

namespace cinekine { namespace ovengine {

Block::Block() :
    _granularity(0)
{
}

Block::Block(const char* name, int granularity,
             Layer layer,
             BuilderPaintStyle paintStyle,
             const Allocator& allocator) :
    _allocator(allocator),
    _type(layer),
    _paintStyle(paintStyle),
    _name(name),
    _granularity(granularity)
{
}

void Block::enableGrid(Class classId)
{
    int dim = 0;
    switch (classId)
    {
    case kClass_1x1:    dim = 1;    break;
    case kClass_2x2:    dim = 2;    break;
    case kClass_3x3:    dim = 3;    break;
    case kClass_4x4:    dim = 4;    break;
    default:                        break;
    }
    if (dim <= 0)
        return;
    if (_grids[classId])
        return;

    _grids[classId] = createGrid(dim);
}


auto Block::createGrid(int dimension) -> Grid
{
    return std::move(Grid(dimension*_granularity, dimension*_granularity,
                          _allocator));
}

}   /* namespace ovengine */ }  /* namespace cinekine */
