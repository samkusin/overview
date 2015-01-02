//
//  GameMapGenerator.cpp
//  Overview
//
//  Created by Samir Sinha on 10/16/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Builder/GameMapGenerator.hpp"
#include "Shared/GameTemplates.hpp"
#include "Shared/StaticWorldMap.hpp"

#include "Engine/Model/TileGridMap.hpp"
#include "Engine/Model/RoomGraph.hpp"
#include "Engine/Builder/RoomBuilder.hpp"
#include "Engine/Builder/GridBuilder.hpp"
#include "Engine/Builder/BlockCollectionLoader.hpp"
#include "Engine/Builder/BlockCollection.hpp"
#include "Core/FileStreamBuf.hpp"
#include "Core/StreamBufRapidJson.hpp"

namespace cinek { namespace overview {


    unique_ptr<StaticWorldMap> generateMapFromTemplates
    (
        const GameTemplates& gameTemplates,
        const GenerateMapParams& params
    )
    {
        Allocator allocator = gameTemplates.allocator();
        
        overview::BlockLibrary blockLibrary(128, allocator);
        FileStreamBuf dbStream(params.blocksPathname);
        if (!dbStream)
            return allocate_unique<StaticWorldMap>(allocator, gameTemplates);

        overview::BlockCollectionLoader loader([&blockLibrary](overview::BlockCollection&& collection)
                                               {
                                                   blockLibrary.mapCollectionToSlot(std::move(collection), 0);
                                               },
                                               allocator);

        overview::unserializeFromJSON(dbStream, loader);

        auto tileGridMap = allocate_unique<overview::TileGridMap>(allocator,
                                                                  params.floorX, params.floorY,
                                                                  params.overlayToFloorRatio,
                                                                  allocator);
        auto roomGraph = allocate_unique<overview::RoomGraph>(allocator,
                                                              params.roomLimit,
                                                              allocator);

        auto& blockCollection = blockLibrary.collectionAtSlot(0);
        auto tileCollectionSlot = gameTemplates.tileLibrary().slotByCollectionName("dungeon");
/*
        overview::GridBuilder floorBuilder(tileGridMap->floor(), tileGridMap->overlayToFloorRatio());
        auto floorDims = floorBuilder.dimensions();
        floorBuilder.fillBox(overview::RoomVertex(0,0),
                             floorDims,
                             blockCollection["dirt_ground"], tileCollectionSlot);

        //  Construct our rooms
        auto room = roomGraph->root();
        overview::RoomAABB roomAABB(overview::RoomVertex(12,12),
                                    overview::RoomVertex(36,36));
        room.resetToBounds(roomAABB);

        auto portal = room.portal(overview::kRoomSide_West);
        portal.setFromOffsets(0, 0);

        portal = room.portal(overview::kRoomSide_North);
        portal.setFromOffsets(0, 0);

        portal = room.portal(overview::kRoomSide_East);
        portal.setFromOffsets(8, 4);

        roomAABB.min = portal.startPos() - overview::RoomVertex(0,4);
        roomAABB.max = portal.endPos() + overview::RoomVertex(8,4);

        auto eastRoom = portal.createRoom(roomAABB);
        portal = eastRoom.portal(overview::kRoomSide_West);
        portal.setAsOpen();

        portal = room.portal(overview::kRoomSide_South);
        portal.setFromOffsets(8, 4);

        roomAABB.min = portal.startPos() - overview::RoomVertex(4,0);
        roomAABB.max = portal.endPos() + overview::RoomVertex(12,12);

        auto southRoom = portal.createRoom(roomAABB);
        portal = southRoom.portal(overview::kRoomSide_North);
        portal.setAsOpen();

        //  paint all of the rooms
        overview::room_builder::PaintStyle style;

        style.floorBlockName = "wood_tile";
        style.wallBlockName = "brick_wall";

        overview::room_builder::paint(room, *tileGridMap,
                                      style, blockCollection, tileCollectionSlot);

        style.floorBlockName = "wood_floor";
        style.wallBlockName = "brick_wall";

        overview::room_builder::paint(eastRoom, *tileGridMap,
                                      style, blockCollection, tileCollectionSlot);
        overview::room_builder::paint(southRoom, *tileGridMap,
                                      style, blockCollection, tileCollectionSlot);
*/
        GridBuilder overlayBuilder(tileGridMap->overlay(), 1);
        auto& wallBlock = blockCollection["brick_wall"];
        overlayBuilder.drawLine(glm::ivec2(12,4), 4, GridBuilder::kDrawDirection_Vertical,
                         wallBlock, GridBuilder::kBlockSide_Right, tileCollectionSlot);
        
        auto staticWorldMap = allocate_unique<StaticWorldMap>(allocator, gameTemplates);

        staticWorldMap->loadTileGridMap(std::move(tileGridMap));
        staticWorldMap->loadRoomGraph(std::move(roomGraph));

        return std::move(staticWorldMap);
    }


} /* namespace overview */ } /* namespace cinek */
