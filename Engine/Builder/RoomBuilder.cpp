//
//  RoomBuilder.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Builder/RoomBuilder.hpp"
#include "Engine/Builder/GridBuilder.hpp"
#include "Engine/Model/TileGridMap.hpp"

namespace cinek {
    namespace overview {
        namespace room_builder {

void paint(Room room, TileGridMap& grid,
           const PaintStyle& style,
           const BlockCollection& blockCollection,
           TileSlot tileCollectionSlot)
{
    GridBuilder floorBuilder(grid.floor(), grid.overlayToFloorRatio());
    GridBuilder overlayBuilder(grid.overlay(), 1);

    // paint edges based on portal points
    auto roomAABB = room.aabb();
    auto& floorBlock = blockCollection[style.floorBlockName];
    floorBuilder.fillBox(roomAABB.min, roomAABB.dimensions(), floorBlock, tileCollectionSlot);

    auto& wallBlock = blockCollection[style.wallBlockName];

    //  top side
    auto portal = room.portal(kRoomSide_North);
    auto len = portal ? portal.startPos().x - roomAABB.min.x : roomAABB.max.x - roomAABB.min.x;
    if (len > 0)
    {
        overlayBuilder.drawLine(roomAABB.min, len, GridBuilder::kDrawDirection_Horizontal,
                         wallBlock, GridBuilder::kBlockSide_Top, tileCollectionSlot);
    }
    len = portal ? roomAABB.max.x - portal.endPos().x : 0;
    if (len > 0)
    {
        overlayBuilder.drawLine(portal.endPos(), len, GridBuilder::kDrawDirection_Horizontal,
                          wallBlock, GridBuilder::kBlockSide_Top, tileCollectionSlot);
    }
    portal = room.portal(kRoomSide_South);
    len = portal ? portal.startPos().x - roomAABB.min.x : roomAABB.max.x - roomAABB.min.x;
    if (len > 0)
    {
        auto startPos = roomAABB.min;
        startPos.y = roomAABB.max.y;
        overlayBuilder.drawLine(startPos, len, GridBuilder::kDrawDirection_Horizontal,
                         wallBlock, GridBuilder::kBlockSide_Bottom, tileCollectionSlot);
    }
    len = portal ? roomAABB.max.x - portal.endPos().x : 0;
    if (len > 0)
    {
        overlayBuilder.drawLine(portal.endPos(), len, GridBuilder::kDrawDirection_Horizontal,
                         wallBlock, GridBuilder::kBlockSide_Bottom, tileCollectionSlot);
    }
    portal = room.portal(kRoomSide_East);
    len = portal ? portal.startPos().y - roomAABB.min.y : roomAABB.max.y - roomAABB.min.y;
    if (len > 0)
    {
        auto startPos = roomAABB.max;
        startPos.y = roomAABB.min.y;
        overlayBuilder.drawLine(startPos, len, GridBuilder::kDrawDirection_Vertical,
                         wallBlock, GridBuilder::kBlockSide_Right, tileCollectionSlot);
    }
    len = portal ? roomAABB.max.y - portal.endPos().y : 0;
    if (len > 0)
    {
        overlayBuilder.drawLine(portal.endPos(), len, GridBuilder::kDrawDirection_Vertical,
                         wallBlock, GridBuilder::kBlockSide_Right, tileCollectionSlot);
    }
    portal = room.portal(kRoomSide_West);
    len = portal ? portal.startPos().y - roomAABB.min.y : roomAABB.max.y - roomAABB.min.y;
    if (len > 0)
    {
        overlayBuilder.drawLine(roomAABB.min, len, GridBuilder::kDrawDirection_Vertical,
                         wallBlock, GridBuilder::kBlockSide_Left, tileCollectionSlot);
    }
    len = portal ? roomAABB.max.y - portal.endPos().y : 0;
    if (len > 0)
    {
        overlayBuilder.drawLine(portal.endPos(), len, GridBuilder::kDrawDirection_Vertical,
                         wallBlock, GridBuilder::kBlockSide_Left, tileCollectionSlot);
    }
}
        } /* namespace room_builder */
    } /* namespace overview */
} /* namespace cinek */