//
//  RoomGraph.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Model/RoomGraph.hpp"

namespace cinekine { namespace ovengine {

RoomGraph::RoomGraph(size_t maxRooms, const Allocator& allocator) :
    _root(),
    _roomPool(maxRooms, allocator),
    _portalPool(maxRooms * kRoomSide_Count, allocator)
{
    _root = createRoom();
}

void RoomGraph::invalidate()
{
    _portalPool.destructAll();
    _roomPool.destructAll();
    _root = Room();
}

Room RoomGraph::createRoom()
{
    auto info = _roomPool.allocate();

    auto portal = _portalPool.allocate();
    portal->fromRoom = info;
    portal->side = kRoomSide_North;
    info->portals[portal->side] = portal;

    portal = _portalPool.allocate();
    portal->fromRoom = info;
    portal->side = kRoomSide_East;
    info->portals[portal->side] = portal;

    portal = _portalPool.allocate();
    portal->fromRoom = info;
    portal->side = kRoomSide_South;
    info->portals[portal->side] = portal;

    portal = _portalPool.allocate();
    portal->fromRoom = info;
    portal->side = kRoomSide_West;
    info->portals[portal->side] = portal;

    return Room(reinterpret_cast<uintptr_t>(this), info);
}

Room RoomGraph::root() const
{
    return _root;
}



}   /* namespace ovengine */ }  /* namespace cinekine */
