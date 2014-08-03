//
//  StageGenerator.cpp
//  Overview
//
//  Created by Samir Sinha on 8/3/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Game/StageGenerator.hpp"
#include "Engine/Model/Stage.hpp"
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
        _floorBuilder(_stage.tileGridMap().floor(),
                     _stage.tileLibrary(),
                     _blockLibrary,
                     _allocator),
        _overlayBuilder(_stage.tileGridMap().overlay(),
                        _stage.tileLibrary(),
                        _blockLibrary,
                        _allocator)
    {
        
        FileStreamBuf dbStream("blocks_dungeon.json");
        if (!dbStream)
            return;

        ovengine::BlockCollectionLoader loader([this](ovengine::BlockCollection&& collection)
                                               {
                                                   _blockLibrary.mapCollectionToSlot(std::move(collection), 0);
                                               },
                                               _allocator);
        
        ovengine::unserializeFromJSON(dbStream, loader);
        
        //  start map generation
        _floorBuilder.clear();
        _overlayBuilder.clear();
        
        auto floorDims = _floorBuilder.dimensions();
        
        _floorBuilder.setBlockCollection(0);
        _floorBuilder.paintRect("dirt_ground", 0, 0, floorDims.x, floorDims.y);
        _floorBuilder.paintRect("wood_floor",
                                floorDims.x/4, floorDims.y/4,
                                floorDims.x - floorDims.x/4, floorDims.y - floorDims.y/4);
    }
    
    void StageGenerator::update()
    {
        
    }
    
    
} /* namespace prototype */ } /* namespace cinekine */
