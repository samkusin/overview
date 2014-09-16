//
//  RoomGraph::Impl.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "./RoomGraphImpl.hpp"

namespace cinekine { namespace ovengine {

RoomGraph::Impl::Impl(size_t maxRooms,
                                       const Allocator& allocator) :
    _root(nullptr),
    _roomPool(maxRooms, allocator),
    _portalPool(maxRooms * kRoomSide_Count, allocator),
    _portalVertices(allocator)
{
    _portalVertices.reserve(maxRooms * kRoomSide_Count);
    _root = createRoomNode();
}

void RoomGraph::Impl::invalidate()
{
    _portalVertices.clear();
    _portalPool.destructAll();
    _roomPool.destructAll();
    _root = createRoomNode();
}

Room RoomGraph::Impl::createRoom()
{
    return Room(reinterpret_cast<uintptr_t>(this), createRoomNode());
}

RoomNode* RoomGraph::Impl::createRoomNode()
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

    return info;
}

void RoomGraph::Impl::initPortalNode(PortalNode* node, RoomNode* fromRoom, RoomSide side)
{
    node->fromRoom = fromRoom;
    node->side = side;
    node->iV0 = _portalVertices.size();
    _portalVertices.emplace_back();
    node->iV1 = _portalVertices.size();
    _portalVertices.emplace_back();

    fromRoom->portals[side] = node;
}


Room RoomGraph::Impl::root() const
{
    return Room(reinterpret_cast<uintptr_t>(this), _root);
}



}   /* namespace ovengine */ }  /* namespace cinekine */
