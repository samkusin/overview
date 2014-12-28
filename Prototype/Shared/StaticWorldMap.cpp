//
//  StaticWorldMap.cpp
//  Overview
//
//  Created by Samir Sinha on 12/23/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Shared/StaticWorldMap.hpp"

#include "Engine/Model/TileGridMap.hpp"
#include "Engine/Model/RoomGraph.hpp"
#include "Engine/Model/TileCollectionLoader.hpp"
#include "Graphics/BitmapLibrary.hpp"
#include "Core/FileStreamBuf.hpp"
#include "Core/StreamBufRapidJson.hpp"

namespace cinek { namespace overview {

StaticWorldMap::StaticWorldMap(const GameTemplates& templates) :
    _templates(&templates)
{
}

StaticWorldMap::~StaticWorldMap() = default;

void StaticWorldMap::loadTileGridMap(unique_ptr<overview::TileGridMap>&& tileGridMap)
{
    _tileGridMap = std::move(tileGridMap);
}

void StaticWorldMap::loadRoomGraph(unique_ptr<overview::RoomGraph>&& roomGraph)
{
    _roomGraph = std::move(roomGraph);
}

AABB<Point> StaticWorldMap::bounds() const
{
    AABB<Point> aabb;
    auto gridDims = _tileGridMap->overlayDimensions();
    aabb.min = Point(0,0,0);
    aabb.max = Point(gridDims.x, gridDims.y, _tileGridMap->overlayToFloorRatio());
    return aabb;
}

} /* namespace overview */ } /* namespace cinek */
