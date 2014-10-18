//
//  GameMapGenerator.cpp
//  Overview
//
//  Created by Samir Sinha on 10/16/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Builder/GameMapGenerator.hpp"
#include "Shared/GameTemplates.hpp"

#include "Engine/Model/TileGridMap.hpp"
#include "Engine/Model/RoomGraph.hpp"
#include "Engine/Builder/RoomBuilder.hpp"
#include "Engine/Builder/GridBuilder.hpp"
#include "Engine/Builder/BlockCollectionLoader.hpp"
#include "Engine/Builder/BlockCollection.hpp"
#include "Core/FileStreamBuf.hpp"
#include "Core/StreamBufRapidJson.hpp"

namespace cinekine { namespace prototype {
    
    
    ovengine::GameTemplates& generateMapFromTemplates(
                                            ovengine::GameTemplates& gameTemplates,
                                            const GenerateMapParams& params)
    {
        Allocator& allocator = gameTemplates.allocator();

        ovengine::BlockLibrary blockLibrary(128, allocator);
        FileStreamBuf dbStream(params.blocksPathname);
        if (!dbStream)
            return gameTemplates;
        
        ovengine::BlockCollectionLoader loader([&blockLibrary](ovengine::BlockCollection&& collection)
                                               {
                                                   blockLibrary.mapCollectionToSlot(std::move(collection), 0);
                                               },
                                               allocator);
        
        ovengine::unserializeFromJSON(dbStream, loader);
        
        auto tileGridMap = allocate_unique<ovengine::TileGridMap>(allocator,
                                                                  params.floorX, params.floorY,
                                                                  params.overlayToFloorRatio,
                                                                  allocator);
        auto roomGraph = allocate_unique<ovengine::RoomGraph>(allocator,
                                                              params.roomLimit,
                                                              allocator);
        
        auto& blockCollection = blockLibrary.collectionAtSlot(0);
        auto tileCollectionSlot = gameTemplates.tileLibrary().slotByCollectionName("dungeon");
        
        ovengine::GridBuilder floorBuilder(tileGridMap->floor(), tileGridMap->overlayToFloorRatio());
        auto floorDims = floorBuilder.dimensions();
        floorBuilder.fillBox(ovengine::RoomVertex(0,0),
                             floorDims,
                             blockCollection["dirt_ground"], tileCollectionSlot);
        
        //  Construct our rooms
        auto room = roomGraph->root();
        ovengine::RoomAABB roomAABB(ovengine::RoomVertex(12,12),
                                    ovengine::RoomVertex(36,36));
        room.resetToBounds(roomAABB);
        
        auto portal = room.portal(ovengine::kRoomSide_West);
        portal.setFromOffsets(0, 0);
        
        portal = room.portal(ovengine::kRoomSide_North);
        portal.setFromOffsets(0, 0);
        
        portal = room.portal(ovengine::kRoomSide_East);
        portal.setFromOffsets(8, 4);
        
        roomAABB.min = portal.startPos() - ovengine::RoomVertex(0,4);
        roomAABB.max = portal.endPos() + ovengine::RoomVertex(8,4);
        
        auto eastRoom = portal.createRoom(roomAABB);
        portal = eastRoom.portal(ovengine::kRoomSide_West);
        portal.setAsOpen();
        
        portal = room.portal(ovengine::kRoomSide_South);
        portal.setFromOffsets(8, 4);
        
        roomAABB.min = portal.startPos() - ovengine::RoomVertex(4,0);
        roomAABB.max = portal.endPos() + ovengine::RoomVertex(12,12);
        
        auto southRoom = portal.createRoom(roomAABB);
        portal = southRoom.portal(ovengine::kRoomSide_North);
        portal.setAsOpen();
        
        //  paint all of the rooms
        ovengine::room_builder::PaintStyle style;
        
        style.floorBlockName = "wood_tile";
        style.wallBlockName = "brick_wall";
        
        ovengine::room_builder::paint(room, *tileGridMap,
                                      style, blockCollection, tileCollectionSlot);
        
        style.floorBlockName = "wood_floor";
        style.wallBlockName = "brick_wall";
        
        ovengine::room_builder::paint(eastRoom, *tileGridMap,
                                      style, blockCollection, tileCollectionSlot);
        ovengine::room_builder::paint(southRoom, *tileGridMap,
                                      style, blockCollection, tileCollectionSlot);
        
        gameTemplates.loadTileGridMap(std::move(tileGridMap));
        gameTemplates.loadRoomGraph(std::move(roomGraph));
        
        return gameTemplates;
    }
    
    
} /* namespace prototype */ } /* namespace cinekine */
