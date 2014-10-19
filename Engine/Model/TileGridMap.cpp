//
//  TileGridMap.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Model/TileGridMap.hpp"

namespace cinek { namespace overview {

    TileGridMap::TileGridMap(uint32_t xDimension, uint32_t yDimension,
                             uint32_t overlayToFloorRatio,
                             const Allocator& allocator) :
        _floorGrid(yDimension, xDimension, allocator),
        _overlayGrid(yDimension*overlayToFloorRatio, xDimension*overlayToFloorRatio, allocator),
        _overlayToFloorRatio(overlayToFloorRatio)
    {
    }

    TileGridMap::TileGridMap() :
        _floorGrid(),
        _overlayGrid(),
        _overlayToFloorRatio(1.f)
    {
    }

    TileGridMap::TileGridMap(TileGridMap&& other) :
        _floorGrid(std::move(other._floorGrid)),
        _overlayGrid(std::move(other._overlayGrid)),
        _overlayToFloorRatio(other._overlayToFloorRatio)
    {
        other._overlayToFloorRatio = 1.f;
    }

    TileGridMap& TileGridMap::operator=(TileGridMap&& other)
    {
        _floorGrid = std::move(other._floorGrid);
        _overlayGrid = std::move(other._overlayGrid);
        _overlayToFloorRatio = other._overlayToFloorRatio;

        other._overlayToFloorRatio = 1.f;

        return *this;
    }


} /* namespace overview */ } /* namespace cinek */
