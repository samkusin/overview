//
//  RoomGraph.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/Model/RoomGraph.hpp"

#include "./Internal/RoomGraphImpl.hpp"

namespace cinekine { namespace ovengine {


RoomGraph::RoomGraph(size_t maxRooms, const Allocator& allocator)
{
    Allocator internalAllocator { allocator };
    _impl = unique_ptr<Impl>(internalAllocator.newItem<Impl>(maxRooms, allocator),
                                           internalAllocator);
}

//  implementation here to allow for unique_ptr to the impl
RoomGraph::~RoomGraph() = default;

RoomGraph::RoomGraph(RoomGraph&& other) :
    _impl(std::move(other._impl))
{
}

RoomGraph& RoomGraph::operator=(RoomGraph&& other)
{
    _impl = std::move(other._impl);
    return *this;
}


void RoomGraph::invalidate()
{
    _impl->invalidate();
}

Room RoomGraph::createRoom()
{
    return _impl->createRoom();
}

Room RoomGraph::root() const
{
    return _impl->root();
}



}   /* namespace ovengine */ }  /* namespace cinekine */
