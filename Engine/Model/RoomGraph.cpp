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
    _portalPool(maxRooms * kRoomSide_Count, allocator),
    _portalVertices(allocator)
{
    _portalVertices.reserve(maxRooms * kRoomSide_Count);
    _root = createRoom();
}

void RoomGraph::invalidate()
{
    _portalVertices.clear();
    _portalPool.destructAll();
    _roomPool.destructAll();
    _root = createRoom();
}

Room RoomGraph::createRoom()
{
    auto info = _roomPool.allocate();

    auto portal = _portalPool.allocate();
    initPortalNode(portal, info, kRoomSide_North);

    portal = _portalPool.allocate();
    initPortalNode(portal, info, kRoomSide_East);

    portal = _portalPool.allocate();
    initPortalNode(portal, info, kRoomSide_South);

    portal = _portalPool.allocate();
    initPortalNode(portal, info, kRoomSide_West);

    return Room(reinterpret_cast<uintptr_t>(this), info);
}

void RoomGraph::initPortalNode(PortalNode* node, RoomNode* fromRoom, RoomSide side)
{
    node->fromRoom = fromRoom;
    node->side = side;
    node->iV0 = _portalVertices.size();
    _portalVertices.emplace_back();
    node->iV1 = _portalVertices.size();
    _portalVertices.emplace_back();

    fromRoom->portals[side] = node;
}


Room RoomGraph::root() const
{
    return _root;
}



}   /* namespace ovengine */ }  /* namespace cinekine */
