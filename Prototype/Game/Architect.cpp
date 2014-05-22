//
//  Architect.cpp
//  Overview
//
//  Created by Samir Sinha on 5/8/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Architect.hpp"
#include "Engine/Builder/BuilderTypes.hpp"
#include "Engine/Model/MapTypes.hpp"

#include <cstdlib>

namespace cinekine { namespace prototype {
    
        
    //  determines whether we can build a room from the specified box region
    bool Architect::canBuildRoom(const ovengine::Box& bounds,
                                 const ovengine::Box& box) const
    {
        if (!box.inside(bounds))
            return false;
        
        for (auto& room : _rooms)
        {
            if (room.box.intersects(box))
                return false;
        }
        return true;
    }
    
    //  randomize a box.  one or more boxes comprise a room
    auto Architect::generateBox(int32_t wMin, int32_t hMin,
                                int32_t wMax, int32_t hMax) -> ovengine::Box
    {
        ovengine::Box box;
        int32_t xSize = wMin + (rand() % (wMax-wMin)+1);
        int32_t ySize = hMin + (rand() % (hMax-hMin)+1);
        //  define a random segment within the supplied bounding segment
        box.p0 = glm::ivec2();
        box.p1 = glm::ivec2(xSize, ySize);
        return box;
    }
        

    Architect::Architect(ovengine::Map& map,
                         const rendermodel::TileDatabase& tileTemplates,
                         const Allocator& allocator) :
        _allocator(allocator),
        _state(kState_BuildRegions),
        _tileTemplates(tileTemplates),
        _map(map),
        _builder(_allocator.newItem<ovengine::Builder>(_map,
                                                       _tileTemplates,
                                                       256,
                                                       _allocator), _allocator),
        _rooms(std_allocator<Room>(_allocator))
    {
    }
        
    void Architect::update()
    {
        switch (_state)
        {
            case kState_BuildRegions:
            {
                newRegion({
                    ovengine::Box::Point(0, 0),
                    ovengine::Box::Point((int32_t)_map.bounds().xUnits, (int32_t)_map.bounds().yUnits)
                });
            }
                break;
            case kState_BuildConnections:
            {
                //  take last two regions and build a connection
                //
                uint32_t roomIndexA = (uint32_t)_rooms.size() - 2;
                uint32_t roomIndexB = (uint32_t)_rooms.size() - 1;
                newConnection(roomIndexA, roomIndexB);
            }
                break;
            default:
                break;
        }
    }
        
    void Architect::newRegion(const ovengine::Box& mapBounds)
    {
        //  prioritize large rooms.  If our randomized room selection cannot
        //  fit a 3x3 room minimum, then we (likely) have no room to build
        int32_t boxMinW = 3+(rand() % 5) , boxMinH = 3+(rand() % 5);
        bool widthDominant = true;
        auto roomBox = ovengine::Box();
        
        while (boxMinW >= 3 && boxMinH >= 3)
        {
            roomBox = generateBox(
                                  boxMinW, boxMinH,
                                  boxMinW + boxMinW/2,
                                  boxMinH + boxMinH/2);
            
            //  randomize our box origin - this is the simplest way to
            //  position a box.  basically in this implementation, there's no
            //  design to our room layout
            int xVariance = mapBounds.width() - roomBox.width();
            int yVariance = mapBounds.height() - roomBox.height();
            int attempts = 0;
            bool boxAllowed = false;
            ovengine::Box::Point roomPt1 = roomBox.p1;
            const int kMaxAttempts = 16;
            do
            {
                roomBox = mapBounds;
                if (xVariance > 0)
                    roomBox.p0.x += (rand() % (xVariance+1));
                if (yVariance > 0)
                    roomBox.p0.y += (rand() % (yVariance+1));
                roomBox.p1 = roomPt1 + roomBox.p0;
                ++attempts;
            }
            while(!(boxAllowed = canBuildRoom(mapBounds, roomBox))
                  && attempts < kMaxAttempts);
            if (boxAllowed)
                break;
            
            roomBox.clear();
            
            //  shrink our box size since our (admittedly weak) box test
            //  failed.  then try again.
            if (widthDominant)
            {
                boxMinH -= 1;
            }
            else
            {
                boxMinW -= 1;
            }
            widthDominant = !widthDominant;
        }
        
        ovengine::TileBrush floorBrush = { 1, 2 };
        ovengine::TileBrush wallBrush = { 1, 1 };
        vector<ovengine::NewRegionInstruction> instructions;
        
        if (roomBox)
        {
            auto policy = ovengine::NewRegionInstruction::kRandomize;
            instructions.emplace_back(policy);
            
            auto& instruction = instructions.back();
            instruction.box = roomBox;
            instruction.terminal = true;
            
            _rooms.emplace_back();
            auto& room = _rooms.back();
            room.handle = _builder->makeRegion(floorBrush, wallBrush, instructions);
            room.box = roomBox;
            
            //  connect room pairs
            if (!(_rooms.size() % 2))
            {
                _state = kState_BuildConnections;
            }
        }
        else
        {
            _state = kState_Done;
        }
    }
    
    void Architect::newConnection(uint32_t roomA, uint32_t roomB)
    {
        ovengine::TileBrush floorBrush = { 1, 2 };
        ovengine::TileBrush wallBrush = { 1, 1 };
        vector<ovengine::NewRegionInstruction> instructions;
        
        vector<ovengine::MapPoint> points;
        _builder->connectRegions(floorBrush, wallBrush, _rooms[roomA].handle, _rooms[roomB].handle, points);
        _state = kState_BuildRegions;
    }

    
} /* namespace prototype */ } /* namespace cinekine */
