//
//  StageGenerator.cpp
//  Overview
//
//  Created by Samir Sinha on 8/3/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Game/StageGenerator.hpp"
#include "Engine/Model/World.hpp"
#include "Engine/Model/TileLibrary.hpp"
#include "Engine/Model/TileGridMap.hpp"
#include "Engine/Model/RoomGraph.hpp"
#include "Engine/Builder/RoomBuilder.hpp"
#include "Engine/Builder/GridBuilder.hpp"
#include "Engine/Builder/BlockCollectionLoader.hpp"
#include "Engine/Builder/BlockCollection.hpp"
#include "Core/FileStreamBuf.hpp"
#include "Core/StreamBufRapidJson.hpp"

namespace cinekine { namespace prototype {
    
   
    StageGenerator::StageGenerator(const Allocator& allocator) :
        _allocator(allocator),
        _blockLibrary(32, _allocator)
    {
        
        FileStreamBuf dbStream("blocks.json");
        if (!dbStream)
            return;

        ovengine::BlockCollectionLoader loader([this](ovengine::BlockCollection&& collection)
                                               {
                                                   _blockLibrary.mapCollectionToSlot(std::move(collection), 0);
                                               },
                                               _allocator);
        
        ovengine::unserializeFromJSON(dbStream, loader);
    }
    
    std::shared_ptr<ovengine::World>  StageGenerator::createWorld(const ovengine::TileLibrary& tileLibrary,
                                                const ovengine::SpriteLibrary& spriteLibrary,
                                                const CreateWorldParams& params)
    {
        ovengine::TileGridMap tileGridMap(params.floorX, params.floorY,
                                          params.overlayToFloorRatio,
                                          _allocator);
        ovengine::RoomGraph roomGraph(params.roomLimit,
                                      _allocator);
        
        auto& blockCollection = _blockLibrary.collectionAtSlot(0);
        auto tileCollectionSlot = tileLibrary.slotByCollectionName("dungeon");
        
        ovengine::GridBuilder floorBuilder(tileGridMap.floor(), tileGridMap.overlayToFloorRatio());
        auto floorDims = floorBuilder.dimensions();
        floorBuilder.fillBox(ovengine::RoomVertex(0,0),
                             floorDims,
                             blockCollection["dirt_ground"], tileCollectionSlot);
        
        //  Construct our rooms
        auto room = roomGraph.root();
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
        
        ovengine::room_builder::paint(room, tileGridMap,
                                      style, blockCollection, tileCollectionSlot);
        
        style.floorBlockName = "wood_floor";
        style.wallBlockName = "brick_wall";
        
        ovengine::room_builder::paint(eastRoom, tileGridMap,
                                      style, blockCollection, tileCollectionSlot);
        ovengine::room_builder::paint(southRoom, tileGridMap,
                                      style, blockCollection, tileCollectionSlot);

        
        return std::allocate_shared<ovengine::World,
                                    std_allocator<ovengine::World>,
                                    ovengine::RoomGraph&&,
                                    ovengine::TileGridMap&&,
                                    const ovengine::TileLibrary&,
                                    const ovengine::SpriteLibrary&,
                                    const Allocator&>
                                    (
                                     _allocator,
                                     std::move(roomGraph),
                                     std::move(tileGridMap),
                                     tileLibrary,
                                     spriteLibrary,
                                     _allocator
                                    );
    }
    
    
} /* namespace prototype */ } /* namespace cinekine */
