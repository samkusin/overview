//
//  StageGenerator.cpp
//  Overview
//
//  Created by Samir Sinha on 8/3/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Game/StageGenerator.hpp"
#include "Engine/Model/Stage.hpp"
#include "Engine/Model/TileLibrary.hpp"
#include "Engine/Model/TileGridMap.hpp"
#include "Engine/Builder/RoomBuilder.hpp"
#include "Engine/Builder/GridBuilder.hpp"
#include "Engine/Builder/BlockCollectionLoader.hpp"
#include "Engine/Builder/BlockCollection.hpp"
#include "Core/FileStreamBuf.hpp"
#include "Core/StreamBufRapidJson.hpp"

namespace cinekine { namespace prototype {
    
   
    StageGenerator::StageGenerator(ovengine::Stage& stage,
                                   const Allocator& allocator) :
        _allocator(allocator),
        _stage(stage),
        _blockLibrary(32, _allocator),
        _roomGraph(128, _allocator)
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
        
        //  start map generation
        _stage.tileGridMap().floor().clear();
        _stage.tileGridMap().overlay().clear();
       
        auto& blockCollection = _blockLibrary.collectionAtSlot(0);
        auto tileCollectionSlot = _stage.tileLibrary().slotByCollectionName("dungeon");
        
        ovengine::GridBuilder floorBuilder(_stage.tileGridMap().floor(), _stage.tileGridMap().overlayToFloorRatio());
        auto floorDims = floorBuilder.dimensions();
        floorBuilder.fillBox(ovengine::RoomVertex(0,0),
                             floorDims,
                             blockCollection["dirt_ground"], tileCollectionSlot);
        
        auto room = _roomGraph.root();
        ovengine::RoomAABB roomAABB(ovengine::RoomVertex(8,8),
                                    ovengine::RoomVertex(24,24));
        room.resetToBounds(roomAABB);
        
        auto portal = room.portal(ovengine::kRoomSide_West);
        portal.setStartPos(room.AABB().min + ovengine::RoomVertex(0,4));
        portal.setEndPos(portal.startPos() + ovengine::RoomVertex(0,4));
        
        portal = room.portal(ovengine::kRoomSide_East);
        portal.setStartPos(room.AABB().max - ovengine::RoomVertex(0,8));
        portal.setEndPos(portal.startPos() + ovengine::RoomVertex(0,4));
        
        ovengine::room_builder::PaintStyle style;
        style.floorBlockName = "wood_tile";
        style.wallBlockName = "brick_wall";
        
        ovengine::room_builder::paint(room, _stage.tileGridMap(),
                                      style, blockCollection, tileCollectionSlot);
        
        
        /*
        
        _floorBuilder.fillBox(blockCollection["dirt_ground"], tileCollectionSlot,
                              glm::ivec2(0,0), floorDims);
        _floorBuilder.fillBox(blockCollection["wood_tile"],
                              tileCollectionSlot,
                              floorDims/4,
                              floorDims/2);
        
        auto overlayDims = _overlayBuilder.dimensions();
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Top, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Horizontal,
                                 glm::ivec2(8,8), 4);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Top, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Horizontal,
                                 glm::ivec2(16,8), 8);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Bottom, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Horizontal,
                                 glm::ivec2(8,24), 8);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Bottom, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Horizontal,
                                 glm::ivec2(20,24), 4);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Left, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Vertical,
                                 glm::ivec2(8,8), 8);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Left, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Vertical,
                                 glm::ivec2(8,20), 4);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Right, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Vertical,
                                 glm::ivec2(24,8), 4);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Right, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Vertical,
                                 glm::ivec2(24,16), 8);
        
        _floorBuilder.fillBox(blockCollection["wood_floor"],
                              tileCollectionSlot,
                              glm::ivec2(0,8),
                              glm::ivec2(8,12));
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Left, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Vertical,
                                 glm::ivec2(8,8), -8);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Right, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Vertical,
                                 glm::ivec2(20,8), -8);
        
        _floorBuilder.fillBox(blockCollection["wood_floor"],
                              tileCollectionSlot,
                              glm::ivec2(0,12),
                              glm::ivec2(8,12));
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Top, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Horizontal,
                                 glm::ivec2(8,12), -8);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Bottom, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Horizontal,
                                 glm::ivec2(8,24), -8);
        

        _floorBuilder.fillBox(blockCollection["wood_floor"],
                              tileCollectionSlot,
                              glm::ivec2(12,24),
                              glm::ivec2(12,8));
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Left, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Vertical,
                                 glm::ivec2(12,24), 8);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Right, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Vertical,
                                 glm::ivec2(24,24), 8);
        
        _floorBuilder.fillBox(blockCollection["wood_floor"],
                              tileCollectionSlot,
                              glm::ivec2(24,8),
                              glm::ivec2(8,12));
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Top, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Horizontal,
                                 glm::ivec2(24,8), 8);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Bottom, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Horizontal,
                                 glm::ivec2(24,20), 8);
        
        */
    }
    
    void StageGenerator::update()
    {
        
    }
    
    
} /* namespace prototype */ } /* namespace cinekine */
