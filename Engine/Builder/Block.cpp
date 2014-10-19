//
//  Block.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Builder/Block.hpp"

namespace cinek { namespace overview {

Block::Block() :
    _granularity(0)
{
}

Block::Block(const char* name, int granularity, int border,
             Layer layer,
             BuilderPaintStyle paintStyle,
             const Allocator& allocator) :
    _allocator(allocator),
    _type(layer),
    _paintStyle(paintStyle),
    _name(name),
    _granularity(granularity),
    _border(border)
{
}

void Block::enableGrid(GridDimension xGrid, GridDimension yGrid)
{
    int xDim = xGrid;
    int yDim = yGrid;
    if (xDim <= 0 || yDim <= 0 || xDim > kGrid_MaxSize || yDim > kGrid_MaxSize)
        return;
    if (_grids[yGrid-1][xGrid-1])
        return;
    _grids[yGrid-1][xGrid-1] = createGrid(xDim, yDim);
}

auto Block::createGrid(int xDim, int yDim) -> Grid
{
    return std::move(Grid(xDim*_granularity, yDim*_granularity,
                          _allocator));
}

auto Block::findLargestGridByDimensions(GridDimension xGrid, GridDimension yGrid) const
     -> const Block::Grid&
{
    //  choose the grid with the greatest area
    auto blockUnits = std::max(xGrid, yGrid);
    blockUnits = std::min(blockUnits, (GridDimension)Block::kGrid_MaxSize);

    int32_t largestYGrid = 0, largestXGrid = 0;

    for (int32_t yGrid = 1; yGrid <= blockUnits; ++yGrid)
    {
        for (int32_t xGrid = 1; xGrid <= blockUnits; ++xGrid)
        {
            if (hasGrid(xGrid,yGrid))
            {
                if (xGrid*yGrid > largestXGrid*largestYGrid)
                {
                    largestXGrid = xGrid;
                    largestYGrid = yGrid;
                }
            }
        }
    }
    return grid(largestXGrid, largestYGrid);
}

}   /* namespace overview */ }  /* namespace cinek */
