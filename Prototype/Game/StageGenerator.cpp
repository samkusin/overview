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
        _floorBuilder(_stage.tileGridMap().floor()),
        _overlayBuilder(_stage.tileGridMap().overlay())
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
        
        auto& blockCollection = _blockLibrary.collectionAtSlot(0);
        auto tileCollectionSlot = _stage.tileLibrary().slotByCollectionName("dungeon");
        
        //  start map generation
        _floorBuilder.clear();
        _overlayBuilder.clear();
        
        auto floorDims = _floorBuilder.dimensions();
        
        _floorBuilder.fillBox(blockCollection["dirt_ground"], tileCollectionSlot,
                               0, 0, floorDims.x, floorDims.y);
        _floorBuilder.fillBox(blockCollection["wood_floor"],
                               tileCollectionSlot,
                               floorDims.x/4, floorDims.y/4,
                               floorDims.x/2, floorDims.y/2);
        
        auto overlayDims = _overlayBuilder.dimensions();
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Top, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Horizontal,
                                 glm::ivec2(8,8), 4);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Bottom, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Horizontal,
                                 glm::ivec2(8,23), 4);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Left, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Vertical,
                                 glm::ivec2(8,8), 4);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Right, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Vertical,
                                 glm::ivec2(23,8), 4);
        
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Bottom, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Horizontal,
                                 glm::ivec2(28,28), 1);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Right, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Vertical,
                                 glm::ivec2(28,28), 1);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Bottom, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Horizontal,
                                 glm::ivec2(31,31), -1);
        _overlayBuilder.drawLine(blockCollection["brick_wall"],
                                 ovengine::GridBuilder::kBlockSide_Right, tileCollectionSlot,
                                 ovengine::GridBuilder::kDrawDirection_Vertical,
                                 glm::ivec2(31,31), -1);
    }
    
    void StageGenerator::update()
    {
        
    }
    
    
} /* namespace prototype */ } /* namespace cinekine */
