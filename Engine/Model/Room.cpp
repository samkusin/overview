//
//  RoomNode.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Model/Room.hpp"
#include "Engine/Model/RoomGraph.hpp"

namespace cinekine {
    namespace ovengine {

void Room::resetToBounds(const RoomAABB& aabb)
{
    if (!_info)
        return;

    _info->box = aabb;
    //  reset portals to the edges of the AAAB, which indicates a zero-sized or
    //  a "closed" portal
    auto p = portal(kRoomSide_North);
    p.setStartPos(aabb.min);
    p.setEndPos(aabb.min);
    p = portal(kRoomSide_South);
    p.setStartPos(aabb.max);
    p.setEndPos(aabb.max);
    p = portal(kRoomSide_East);
    p.setStartPos(aabb.max);
    p.setEndPos(aabb.max);
    p = portal(kRoomSide_West);
    p.setStartPos(aabb.min);
    p.setEndPos(aabb.min);
}

Portal Room::portal(RoomSide side) const
{
    if (!_info)
        return Portal();

    return Portal(_graph, _info->portals[side]);
}

////////////////////////////////////////////////////////////////////////////////

Room Portal::toRoom() const
{
    if (!_info)
        return Room();

    return Room(_graph, _info->toRoom);
}

Room Portal::fromRoom() const
{
    if (!_info)
        return Room();

    return Room(_graph, _info->fromRoom);
}

Room Portal::createRoom(const RoomAABB& aabb)
{
    if (!_info)
        return Room();

    RoomSide toSide;
    if (_info->side == kRoomSide_West)
        toSide = kRoomSide_East;
    else if (_info->side == kRoomSide_East)
        toSide = kRoomSide_West;
    else if (_info->side == kRoomSide_North)
        toSide = kRoomSide_South;
    else if (_info->side == kRoomSide_South)
        toSide = kRoomSide_North;
    else
        return Room();

    auto graph = reinterpret_cast<RoomGraph*>(_graph);
    auto room = graph->createRoom();
    room.resetToBounds(aabb);
    room._info->portals[toSide]->toRoom = _info->fromRoom;

    return room;
}

    } /* namespace overview */
} /* namespace cinekine */
