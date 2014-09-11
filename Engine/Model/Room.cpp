//
//  RoomNode.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Model/Room.hpp"
#include "Engine/Model/RoomGraph.hpp"

#include "Engine/Debug.hpp"

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

void Portal::setFromOffsets(offset_type offset, offset_type length)
{
    if (!(*this) || !_info->fromRoom)
        return;
    if (offset < 0 || length < 0)
    {
        //  negative lengths and offsets not allowed
        CK_ASSERT(false);
        return;
    }

    auto room = _info->fromRoom;
    auto graph = reinterpret_cast<RoomGraph*>(_graph);

    RoomVertex start = room->box.min;
    RoomVertex end;

    //  deal with clipping afterwards
    switch(_info->side)
    {
    case kRoomSide_North:
    case kRoomSide_South:
        {
            if (_info->side == kRoomSide_South)
                start.y = room->box.max.y;

            start.x += offset;
            end = start;
            end.x += length;

            if (end.x > room->box.max.x)
                end.x = room->box.max.x;
        }
        break;

    case kRoomSide_East:
    case kRoomSide_West:
        {
            if (_info->side == kRoomSide_East)
                start.x = room->box.max.x;

            start.y += offset;
            end = start;
            end.y += length;

            if (end.y > room->box.max.y)
                end.y = room->box.max.y;
        }
        break;
    default:
        CK_ASSERT(false);
        end = start;
        break;
    }

    graph->_portalVertices[_info->iV0] = start;
    graph->_portalVertices[_info->iV1] = end;
}

void Portal::setAsOpen()
{
    auto graph = reinterpret_cast<RoomGraph*>(_graph);
    graph->_portalVertices[_info->iV0] = edgeStartPos();
    graph->_portalVertices[_info->iV1] = edgeEndPos();
}

void Portal::setAsClosed()
{
    auto endPos = edgeEndPos();
    auto graph = reinterpret_cast<RoomGraph*>(_graph);
    graph->_portalVertices[_info->iV0] = endPos;
    graph->_portalVertices[_info->iV1] = endPos;
}

RoomVertex Portal::startPos() const
{
    if (!*this)
        return RoomVertex();

    auto graph = reinterpret_cast<RoomGraph*>(_graph);
    return graph->_portalVertices[_info->iV0];
}

void Portal::setStartPos(const RoomVertex& v)
{
    if (!*this)
        return;
    auto graph = reinterpret_cast<RoomGraph*>(_graph);
    graph->_portalVertices[_info->iV0] = v;
}

RoomVertex Portal::endPos() const
{
    if (!*this)
        return RoomVertex();

    auto graph = reinterpret_cast<RoomGraph*>(_graph);
    return graph->_portalVertices[_info->iV1];
}

void Portal::setEndPos(const RoomVertex& v)
{
    if (!*this)
        return;
    auto graph = reinterpret_cast<RoomGraph*>(_graph);
    graph->_portalVertices[_info->iV1] = v;
}

RoomVertex Portal::edgeStartPos() const
{
    RoomVertex vert;
    if (!*this)
        return vert;
    vert = _info->fromRoom->box.min;

    if (_info->side == kRoomSide_East)
        vert.x = _info->fromRoom->box.max.x;
    else if (_info->side == kRoomSide_South)
        vert.y = _info->fromRoom->box.max.y;
    return vert;
}

RoomVertex Portal::edgeEndPos() const
{
    RoomVertex vert;
    if (!*this)
        return vert;
    vert = _info->fromRoom->box.max;

    if (_info->side == kRoomSide_North)
        vert.y = _info->fromRoom->box.min.y;
    else if (_info->side == kRoomSide_West)
        vert.x = _info->fromRoom->box.min.x;
    return vert;
}

    } /* namespace overview */
} /* namespace cinekine */
